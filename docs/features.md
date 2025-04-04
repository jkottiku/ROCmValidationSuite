# ROCmValidationSuite Modules


## GPU Properties – GPUP
The GPU Properties module queries the configuration of a target device and returns the device’s static characteristics. These static values can be used to debug issues such as device support, performance and firmware problems.

## GPU Monitor – GM module [deprecated]
The GPU monitor tool is capable of running on one, some or all of the GPU(s) installed and will report various information at regular intervals. The module can be configured to halt another RVS modules execution if one of the quantities exceeds a specified boundary value.

## PCI Express State Monitor – PESM module  [deprecated]
The PCIe State Monitor tool is used to actively monitor the PCIe interconnect between the host platform and the GPU. The module will register a “listener” on a target GPU’s PCIe interconnect, and log a message whenever it detects a state change. The PESM will be able to detect the following state changes:

1.	PCIe link speed changes
2.	GPU power state changes

## ROCm Configuration Qualification Tool - RCQT module
The ROCm Configuration Qualification Tool ensures the platform is capable of running ROCm applications and is configured correctly. It checks the installed versions of the ROCm components and the platform configuration of the system. This includes checking the dependencies corresponding to the ROCm meta-packages are installed correctly.

## PCI Express Qualification Tool – PEQT module
The PCIe Qualification Tool is used to qualify the PCIe bus on which the GPU is connected. The qualification test will be capable of determining the following characteristics of the PCIe bus interconnect to a GPU:

1.	Support for Gen 3 atomic completers
2.	DMA transfer statistics
3.	PCIe link speed
4.	PCIe link width

## SBIOS Mapping Qualification Tool – SMQT module  [deprecated]
The GPU SBIOS mapping qualification tool is designed to verify that a platform’s SBIOS has satisfied the BAR mapping requirements for VDI and Radeon Instinct products for ROCm support.

## P2P Benchmark and Qualification Tool – PBQT module
The P2P Benchmark and Qualification Tool is designed to provide the list of all GPUs that support P2P and characterize the P2P links between peers. In addition to testing P2P compatibility, this test will perform a peer-to-peer throughput test between all P2P pairs for performance evaluation. The P2P Benchmark and Qualification Tool will allow users to pick a collection of two or more GPUs to run the test. The user will also be able to select whether or not they want to run the throughput test on each of the pairs.

## PCI Express Bandwidth Benchmark – PEBB module
The PCIe Bandwidth Benchmark attempts to saturate the PCIe bus with DMA transfers between system memory and a target GPU card’s memory. The maximum bandwidth obtained is reported to help debug low bandwidth issues. The benchmark should be capable of targeting one, some or all of the GPUs installed in a platform, reporting individual benchmark statistics for each.

## GPU Stress Test - GST module
The GPU Stress Test runs various GEMM computations as workloads to stress the GPU FLOPS performance and check whether it meets the configured target GFLOPS. GEMM workloads shall be configured as either operation type or data type. GEMM based on operation types include SGEMM, DGEMM and HGEMM (Single/Double/Half-precision General Matrix Multiplication) - configured using operation parameter. GEMM based on data types include `fp8`, `i8`, `fp16`, `bf16`, `fp32` and  `tf32` (`xf32`) - configured using data type parameter. The duration of the test is configurable, both in terms of time (how long to run) and iterations (how many times to run).

## Input EDPp Test - IET module
The Input EDPp Test runs GEMM workloads to stress the GPU power (i.e. TGP). This test is used to verify if the GPU is capable of handling max. power stress for a sustained period of time. Also checks whether GPU power reaches a set target power.

## Memory Test - MEM module
The Memory module tests the GPU memory for hard and soft errors using HIP. It consists of various tests that use algorithms like Walking 1 bit, Moving inversion and Modulo 20. The module executes the following memory tests [Algorithm, data pattern]

1. Walking 1 bit
2. Own address test
3. Moving inversions, ones & zeros
4. Moving inversions, 8 bit pattern
5. Moving inversions, random pattern
6. Block move, 64 moves
7. Moving inversions, 32 bit pattern
8. Random number sequence
9. Modulo 20, random pattern
10. Memory stress test

## BABEL benchmark Test - BABEL module
The Babel module executes BabelStream (synthetic GPU benchmark based on the original STREAM benchmark for CPUs) benchmark that measures memory transfer rates (bandwidth) to and from global device memory. Various benchmark tests are implemented using GPU kernels in HIP (Heterogeneous Interface for Portability) programming language.

## Thermal Stress Test - TST module
The Thermal Stress Test (TST) measures/monitors the GPU edge and junction temperatures under various stressful workloads. Also checks whether GPU junction temperature reaches target and trottle temperatures.

