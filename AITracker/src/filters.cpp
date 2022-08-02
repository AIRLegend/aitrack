#include "filters.h"

#include <cstring>

#ifdef OPTIMIZE_MAFilter
#include <cmath>
#endif

MAFilter::MAFilter(int steps, int array_size)
{
	this->n_steps = steps;
	this->array_size = array_size;
	this->idx = 0; // this->idx < this->n_steps

	this->circular_buffer = (float *)new float[steps * array_size]; // float[steps][array_size]
#ifdef OPTIMIZE_MAFilter
	this->sum = (float *)new float[array_size]; // use this array to cache the sum
	for (int i = 0; i < array_size; i++)
		this->sum[i] = nanf("");
#endif
}

MAFilter::~MAFilter()
{
	delete[] this->circular_buffer;
#ifdef OPTIMIZE_MAFilter
	delete[] this->sum;
#endif
}

void MAFilter::filter(float* in_array, float* out_array)
{
	int offset = this->idx * this->array_size;
#ifdef OPTIMIZE_MAFilter
	// equivalent to:
	// typedef float (*CIRCULAR_BUFFER_IDX)[this->array_size];
	// typedef float (*CIRCULAR_BUFFER)[this->n_steps][this->array_size];
	float *circular_buffer_idx = &this->circular_buffer[offset]; // CIRCULAR_BUFFER_IDX circular_buffer_idx = &((CIRCULAR_BUFFER)this->circular_buffer)[this->idx][0];
#endif
	for (int i = 0; i < this->array_size; i++)
	{
#ifdef OPTIMIZE_MAFilter
		if (isnan(this->sum[i]))
		{
			// initialize sum
			this->sum[i] = in_array[i] * this->n_steps;
			// calculate average
			out_array[i] = this->sum[i] / this->n_steps;
			// initialize empty circular_buffer with new value
			for (int j = 0; j < this->n_steps; j++)
			{
				this->circular_buffer[j * this->array_size + i] = in_array[i];
			}
		}
		else
		{
			// Recalculate sum by subtracting old value and adding new value
			this->sum[i] = this->sum[i] - circular_buffer_idx[i] + in_array[i];
			// calculate average
			out_array[i] = this->sum[i] / this->n_steps;
			// Insert current position
			circular_buffer_idx[i] = in_array[i];
		}
#else
		// Insert current position
		this->circular_buffer[offset + i] = in_array[i];
		out_array[i] = 0;

		// get mean of all steps for this position
		for (int j = 0; j < this->n_steps; j++)
		{
			out_array[i] += this->circular_buffer[j * this->array_size + i];
		}

		out_array[i] /= this->n_steps;
#endif
	}

	this->idx = (this->idx + 1) % this->n_steps;
	
}


EAFilter::EAFilter(int array_size)
{
	this->array_size = array_size;
	this->last_value = new float[array_size];
	//If the last value is not 0, the first readings could bounce
	std::memset(last_value, 0, array_size * sizeof(float));  
}

EAFilter::~EAFilter()
{
	delete[] this->last_value;
}

void EAFilter::filter(float* in_array, float* out_array)
{
	for (int i = 0; i < array_size; i++)
	{
		out_array[i] = (0.6f * in_array[i] + 0.4f * this->last_value[i]);
		this->last_value[i] = in_array[i];
	}

}