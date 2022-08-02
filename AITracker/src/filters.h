#pragma once

#define OPTIMIZE_MAFilter 1

class IFilter
{
public:
	virtual void filter(float* in_array, float* out_array)=0;
};


class MAFilter : public IFilter
{
private:
	int array_size;
	int idx;
	int n_steps;

	float *circular_buffer;
#ifdef OPTIMIZE_MAFilter
	float *sum;
#endif


public:
	MAFilter(int steps, int array_size);
	~MAFilter();

	void filter(float* in_array, float* out_array);
};



class EAFilter : public IFilter
{
private:
	int array_size;
	float* last_value;



public:
	EAFilter(int array_size);
	~EAFilter();

	void filter(float* in_array, float* out_array);
};