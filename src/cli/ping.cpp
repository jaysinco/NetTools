#include "core/transport.h"
#include "core/ipv4.h"
#include <iostream>

int main(int argc, char *argv[])
{
    NT_TRY
    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_logtostderr = 1;
    FLAGS_minloglevel = 0;

    if (argc < 2) {
        LOG(ERROR) << "empty target name, please input ip or host name";
        return -1;
    }

    ip4 target_ip;
    std::string target_name = argv[1];
    std::ostringstream ip_desc;
    if (ip4::from_dotted_dec(target_name, &target_ip)) {
        ip_desc << target_ip.to_str();
    } else {
        if (!ip4::from_domain(target_name, &target_ip)) {
            LOG(ERROR) << "invalid ip or host name: " << target_name;
            return -1;
        }
        ip_desc << target_name << " [" << target_ip.to_str() << "]";
    }

    std::cout << "\nPing " << ip_desc.str() << ":" << std::endl;
    auto &apt = adaptor::fit(ip4::zeros);
    pcap_t *handle = transport::open_adaptor(apt);
    constexpr int total_cnt = 4;
    int recv_cnt = 0;
    long sum_cost = 0;
    long min_cost = std::numeric_limits<long>::max();
    long max_cost = std::numeric_limits<long>::min();
    for (int i = 0; i < total_cnt; ++i) {
        std::cout << "Reply from " << target_ip.to_str() << ": ";
        packet reply;
        long cost_ms;
        if (transport::ping(handle, apt, target_ip, reply, cost_ms, 128, "greatjaysinco")) {
            ++recv_cnt;
            min_cost = std::min(min_cost, cost_ms);
            max_cost = std::max(max_cost, cost_ms);
            sum_cost += cost_ms;
            auto &prot = *reply.get_detail().layers.at(1);
            int ttl = dynamic_cast<const ipv4 &>(prot).get_detail().ttl;
            std::cout << "time=" << cost_ms << "ms"
                      << " ttl=" << ttl << std::endl;
        } else {
            std::cout << "timeout" << std::endl;
        }
    }
    std::cout << "\nStatistical information:" << std::endl;
    std::cout << "    packets: sent=" << total_cnt << ", recv=" << recv_cnt
              << ", lost=" << (total_cnt - recv_cnt) << " ("
              << int(float(total_cnt - recv_cnt) / total_cnt * 100) << "% lost)\n";
    if (sum_cost > 0) {
        std::cout << "Estimated time of round trip:" << std::endl;
        std::cout << "    min=" << min_cost << "ms, max=" << max_cost
                  << "ms, avg=" << (sum_cost) / total_cnt << "ms\n";
    }
    NT_CATCH
}
