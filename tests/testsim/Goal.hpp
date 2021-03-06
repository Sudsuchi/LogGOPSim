#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Parser.hpp"

typedef Node* goalop_t;

class Goal {

	private:
		Graph graph;
		uint32_t rank;
		uint32_t num_ranks;

		uint8_t MaxCPU(uint8_t cpu = 0) {
			static uint8_t max_cpu = 0;
			if (cpu > max_cpu) max_cpu = cpu;
			return max_cpu;
		}

		uint8_t MaxNIC(uint8_t nic = 0) {
			static uint8_t max_nic = 0;
			if (nic > max_nic) max_nic = nic;
			return max_nic;
		}

	public:

		goalop_t Send(uint32_t src, uint32_t dest, uint64_t size, uint32_t tag, uint8_t cpu, uint8_t nic) {

			Node* n = graph.addNode();

			n->Type = OPTYPE_SEND;
			n->Peer = dest;
			n->Tag = tag;
			n->Proc = cpu;
			n->Nic = nic;
			n->Size = size;

			MaxCPU(cpu);
			MaxNIC(nic);

			return n;
		}

		goalop_t Recv(uint32_t src, uint32_t dest, uint64_t size, uint32_t tag, uint8_t cpu, uint8_t nic) {

			Node* n = graph.addNode();

			n->Type = OPTYPE_RECV;
			n->Peer = src;
			n->Tag = tag;
			n->Proc = cpu;
			n->Nic = nic;
			n->Size = size;

			MaxCPU(cpu);
			MaxNIC(nic);

			return n;
		}

		goalop_t Calc(uint32_t src, uint64_t size, uint8_t cpu, uint8_t nic) {

			Node* n = graph.addNode();

			n->Type = OPTYPE_CALC;
			n->Peer = 0; // this optype has not real peer, i just set it so it is clearly defined
			n->Tag = 0; // this optype has not real tag, i just set it so it is clearly defined
			n->Proc = cpu;
			n->Nic = nic;
			n->Size = size;

			MaxCPU(cpu);
			MaxNIC(nic);

			return n;
		}

		goalop_t Put(uint32_t src, uint32_t oct, uint32_t dest, uint64_t size, uint32_t tag, uint8_t cpu, uint8_t nic){
			Node* n = graph.addNode();

			n->Type = OPTYPE_PUT;
			n->Peer = dest;
			n->Tag = tag;
			n->Proc = cpu;
			n->Nic = nic;
			n->Size = size;
			n->oct = oct;
            //n->options = options;
            //n->ct = ct;
            //n->threshold = threshold;

			MaxCPU(cpu);
			MaxNIC(nic);

			return n;
		}


		goalop_t Get(uint32_t src, uint32_t oct, uint32_t dest, uint64_t size, uint32_t tag, uint8_t cpu, uint8_t nic){
			Node* n = graph.addNode();

			n->Type = OPTYPE_GET;
			n->Peer = dest;
			n->Tag = tag;
			n->Proc = cpu;
			n->Nic = nic;
			n->Size = size;
			n->oct = oct;
            //n->options = options;
            //n->ct = ct;
            //n->threshold = threshold;

			MaxCPU(cpu);
			MaxNIC(nic);

			return n;
		}


		goalop_t Append(uint32_t src, uint32_t oct, uint32_t allowed, uint64_t size, uint32_t tag, char options, uint8_t cpu, uint8_t nic){
			Node* n = graph.addNode();

			n->Type = OPTYPE_APPEND;
			n->Peer = allowed;
			n->Tag = tag;
			n->Proc = cpu;
			n->Nic = nic;
			n->Size = size;
            n->options = options;
            n->oct = oct;
            //n->ct = ct;
            //n->threshold = threshold;

			MaxCPU(cpu);
			MaxNIC(nic);

			return n;
		}

		goalop_t CTWait(uint32_t src, uint32_t ct, uint64_t test, uint8_t cpu){
			Node* n = graph.addNode();

			n->Type = OPTYPE_CTWAIT;
			n->Peer = 0;
			n->Tag = 0;
			n->ct = ct;
			n->threshold = test;
			n->Proc = cpu;
			n->Nic = 0;

            //n->options = options;
            //n->ct = ct;
            //n->threshold = threshold;

			MaxCPU(cpu);


			return n;
        }

        goalop_t tAppend(uint32_t src, uint32_t oct, uint32_t allowed, uint64_t size, uint32_t tag, char options, uint8_t cpu, uint8_t nic, uint32_t ct, uint64_t threshold){
            Node*n = Append(src, oct, allowed, size, tag, options, cpu, nic);
            n->Type = OPTYPE_TAPPEND;
            n->ct = ct;
            n->threshold = threshold;

            return n;
        }

        goalop_t tPut(uint32_t src, uint32_t oct, uint32_t dest, uint64_t size, uint32_t tag, uint8_t cpu, uint8_t nic, uint32_t ct, uint64_t threshold){
            Node* n = Put(src, oct, dest, size, tag, cpu, nic);
            n->Type = OPTYPE_TPUT;
            n->ct = ct;
            n->threshold = threshold;
            return n;
        }

        goalop_t tGet(uint32_t src, uint32_t oct, uint32_t dest, uint64_t size, uint32_t tag, uint8_t cpu, uint8_t nic, uint32_t ct, uint64_t threshold){
            Node* n = Get(src, oct, dest, size, tag, cpu, nic);
            n->Type = OPTYPE_TGET;
            n->ct = ct;
            n->threshold = threshold;
            return n;
        }


		void StartDependency(goalop_t src, goalop_t dest) {
			// a can not be executed before b is started
			graph.addStartDependency(src, dest);
		}

		void Dependency(goalop_t src, goalop_t dest) {
			//a can not be executed before b is finished
			graph.addDependency(src, dest);
		}

		void SerializeSchedule(char* filename) {

			static int fd;

			// create/open binary schedule if it is the first rank (rank 0)
			if (rank==0) {
				fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
				if (fd == -1) {
					fprintf(stderr, "Couldn't open %s for schedule serialization!\n", filename);
					perror("system error message:");
					exit(EXIT_FAILURE);
				}
			}

			graph.serialize_mmap(fd, rank, num_ranks, MaxCPU(), MaxNIC());

			// close the binary schedule if it is the last rank
			if (rank == num_ranks-1) {
				close(fd);
				sync();
			}

		}

		void SetRank(uint32_t r) {
			rank=r;
		}

		void SetNumRanks(uint32_t nr) {
			num_ranks = nr;
		}

};
