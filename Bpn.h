//  Bpn.h: interface for the CBpn class (MLP)
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPN_H__CB0F7ED8_8AD4_4FD5_B5D9_C60C939F5548__INCLUDED_)
#define AFX_BPN_H__CB0F7ED8_8AD4_4FD5_B5D9_C60C939F5548__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBpn  
{
private:
	int nLayerCnt;		// the # of Layers: Input + Hidden + Output
	int *NodeCnt;		// the # of Nodes: NodeCnt[Layer]
	double **Node;		// Nodes: Node[Layer][Node]
	double ***Weight;	// Weights: Weight[Layer][Node 1][Node 2]
						// Node 1: Current layer, Node 2: Previous layer
	double **Delta;		// delta: Delta[Layer][Node]
	double dGain;		// Learning rate

public:
	CBpn(int nLayerCntInput, int *NodeCntInput, double dGainInput);	// Constructor: Layers, Nodes, Learning rate

	static double ActivationFn(double x);				// Activation
	static double DifferentialActivationFn(double x);	// Differential Activation

	void InitWeight();								// Weight initialization
	int ComputeNet(double *Input);					// Compute network
	void ComputeDelta(double *Output);				// Compute delta
	void UpdateWeight();							// Updage weights
	int Train(double *Input, double *Output);		// Training: ComputeNet + ComputeDelta + UpdateDelta
	virtual ~CBpn();

};

#endif // !defined(AFX_BPN_H__CB0F7ED8_8AD4_4FD5_B5D9_C60C939F5548__INCLUDED_)
