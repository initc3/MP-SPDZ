package utils

type Task struct {
	BlockNumber 	uint64
	TxIndex 		uint
	LogIndex		uint

	EventName		string
	Parameters		[]string
}

type PriorityQueue []*Task

func (pq PriorityQueue) Len() int { return len(pq) }

func (pq PriorityQueue) Less(i, j int) bool {
	if pq[i].BlockNumber != pq[j].BlockNumber {
		return pq[i].BlockNumber < pq[j].BlockNumber
	}
	if pq[i].TxIndex != pq[j].TxIndex {
		return pq[i].TxIndex < pq[j].TxIndex
	}
	return pq[i].LogIndex < pq[j].LogIndex
}

func (pq PriorityQueue) Swap(i, j int) {
	pq[i], pq[j] = pq[j], pq[i]
}

func (pq *PriorityQueue) Push(x interface{}) {
	*pq = append(*pq, x.(*Task))
}

func (pq *PriorityQueue) Pop() interface{} {
	old := *pq
	n := len(old)
	task := old[n-1]
	*pq = old[0 : n-1]
	return task
}