base_url = "https://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/"

satisfiable_cnf_archives = [
    "SAT/RND3SAT/uf20-91.tar.gz",
    "SAT/RND3SAT/uf50-218.tar.gz",
    "SAT/RND3SAT/uf75-325.tar.gz",
    "SAT/RND3SAT/uf100-430.tar.gz",
    "SAT/RND3SAT/uf125-538.tar.gz",
    "SAT/RND3SAT/uf150-645.tar.gz",
    # "SAT/RND3SAT/uf175-753.tar.gz", // these take too long to complete
    # "SAT/RND3SAT/uf200-860.tar.gz",
    # "SAT/RND3SAT/uf225-960.tar.gz",
    # "SAT/RND3SAT/uf250-1065.tar.gz",
    "SAT/BMS/RTI_k3_n100_m429.tar.gz",
    "SAT/BMS/BMS_k3_n100_m429.tar.gz",
    "SAT/CBS/CBS_k3_n100_m403_b10.tar.gz",
    "SAT/CBS/CBS_k3_n100_m403_b30.tar.gz",
    "SAT/CBS/CBS_k3_n100_m403_b50.tar.gz",
    "SAT/CBS/CBS_k3_n100_m403_b70.tar.gz",
    "SAT/CBS/CBS_k3_n100_m403_b90.tar.gz",
    "SAT/CBS/CBS_k3_n100_m411_b10.tar.gz",
    "SAT/CBS/CBS_k3_n100_m411_b30.tar.gz",
    "SAT/CBS/CBS_k3_n100_m411_b50.tar.gz",
    "SAT/CBS/CBS_k3_n100_m411_b70.tar.gz",
    "SAT/CBS/CBS_k3_n100_m411_b90.tar.gz",
    "SAT/CBS/CBS_k3_n100_m418_b10.tar.gz",
    "SAT/CBS/CBS_k3_n100_m418_b30.tar.gz",
    "SAT/CBS/CBS_k3_n100_m418_b50.tar.gz",
    "SAT/CBS/CBS_k3_n100_m418_b70.tar.gz",
    "SAT/CBS/CBS_k3_n100_m418_b90.tar.gz",
    "SAT/CBS/CBS_k3_n100_m423_b10.tar.gz",
    "SAT/CBS/CBS_k3_n100_m423_b30.tar.gz",
    "SAT/CBS/CBS_k3_n100_m423_b50.tar.gz",
    "SAT/CBS/CBS_k3_n100_m423_b70.tar.gz",
    "SAT/CBS/CBS_k3_n100_m423_b90.tar.gz",
    "SAT/CBS/CBS_k3_n100_m429_b10.tar.gz",
    "SAT/CBS/CBS_k3_n100_m429_b30.tar.gz",
    "SAT/CBS/CBS_k3_n100_m429_b50.tar.gz",
    "SAT/CBS/CBS_k3_n100_m429_b70.tar.gz",
    "SAT/CBS/CBS_k3_n100_m429_b90.tar.gz",
    "SAT/CBS/CBS_k3_n100_m435_b10.tar.gz",
    "SAT/CBS/CBS_k3_n100_m435_b30.tar.gz",
    "SAT/CBS/CBS_k3_n100_m435_b50.tar.gz",
    "SAT/CBS/CBS_k3_n100_m435_b70.tar.gz",
    "SAT/CBS/CBS_k3_n100_m435_b90.tar.gz",
    "SAT/CBS/CBS_k3_n100_m441_b10.tar.gz",
    "SAT/CBS/CBS_k3_n100_m441_b30.tar.gz",
    "SAT/CBS/CBS_k3_n100_m441_b50.tar.gz",
    "SAT/CBS/CBS_k3_n100_m441_b70.tar.gz",
    "SAT/CBS/CBS_k3_n100_m441_b90.tar.gz",
    "SAT/CBS/CBS_k3_n100_m449_b10.tar.gz",
    "SAT/CBS/CBS_k3_n100_m449_b30.tar.gz",
    "SAT/CBS/CBS_k3_n100_m449_b50.tar.gz",
    "SAT/CBS/CBS_k3_n100_m449_b70.tar.gz",
    "SAT/CBS/CBS_k3_n100_m449_b90.tar.gz",
    "SAT/GCP/flat30-60.tar.gz",
    "SAT/GCP/flat50-115.tar.gz",
    "SAT/GCP/flat75-180.tar.gz",
    "SAT/GCP/flat100-239.tar.gz",
    "SAT/GCP/flat125-301.tar.gz",
    "SAT/GCP/flat150-360.tar.gz",
    "SAT/GCP/flat175-417.tar.gz",
    "SAT/GCP/flat200-479.tar.gz",
    "SAT/SW-GCP/sw100-8-lp0-c5.tar.gz",
    "SAT/SW-GCP/sw100-8-lp1-c5.tar.gz",
    "SAT/SW-GCP/sw100-8-lp2-c5.tar.gz",
    "SAT/SW-GCP/sw100-8-lp3-c5.tar.gz",
    "SAT/SW-GCP/sw100-8-lp4-c5.tar.gz",
    "SAT/SW-GCP/sw100-8-lp5-c5.tar.gz",
    "SAT/SW-GCP/sw100-8-lp6-c5.tar.gz",
    "SAT/SW-GCP/sw100-8-lp7-c5.tar.gz",
    "SAT/SW-GCP/sw100-8-lp8-c5.tar.gz",
    "SAT/SW-GCP/sw100-8-p0-c5.tar.gz",
    "SAT/PLANNING/BlocksWorld/blocksworld.tar.gz",
    "SAT/PLANNING/Logistics/logistics.tar.gz",
    "SAT/AIS/ais.tar.gz",
    "SAT/DIMACS/LRAN/f.tar.gz",
    "SAT/DIMACS/GCP/gcp-large.tar.gz",
    "SAT/DIMACS/PARITY/parity.tar.gz",
    "SAT/DIMACS/II/inductive-inference.tar.gz",
    "SAT/DIMACS/HANOI/hanoi.tar.gz"
]

unsatisfiable_cnf_archives = [
    "SAT/RND3SAT/uuf50-218.tar.gz",
    "SAT/RND3SAT/uuf75-325.tar.gz",
    # "SAT/RND3SAT/uuf100-430.tar.gz", take too long to run
    # "SAT/RND3SAT/uuf125-538.tar.gz",
    # "SAT/RND3SAT/uuf150-645.tar.gz",
    # "SAT/RND3SAT/uuf175-753.tar.gz",
    # "SAT/RND3SAT/uuf200-860.tar.gz",
    # "SAT/RND3SAT/uuf225-960.tar.gz",
    # "SAT/RND3SAT/uuf250-1065.tar.gz",
    # "SAT/DIMACS/DUBOIS/dubois.tar.gz",  invalid test format
    "SAT/DIMACS/BF/bf.tar.gz",
    "SAT/DIMACS/PHOLE/pigeon-hole.tar.gz",
    "SAT/DIMACS/PRET/pret.tar.gz",
]
