﻿//    Cekirdekler API: a C# explicit multi-device load-balancer opencl wrapper
//    Copyright(C) 2017 Hüseyin Tuðrul BÜYÜKISIK

//   This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.If not, see<http://www.gnu.org/licenses/>.

//#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 200   

#include <vector>
#include <iostream>

#include <CL/cl2.hpp>
#include <utility>
#include <fstream>

#include <chrono>
#include <thread>
#include <time.h>
#include <mutex>

extern "C"
{
	long version = 100040001;
	bool betaVersion = false;
	__declspec(dllexport)
		void repeatKernelNTimes()
	{
		// to do: check if out of order queue works for intel amd nvidia
	}

	__declspec(dllexport)
		void copyMemory(char * dest, char * src, unsigned int count)
	{
		std::copy(src, src + count, dest);
	}

	// stackoverflow
	const char *getErrorString(cl_int error)
	{
		switch (error) {
		case 0: return "CL_SUCCESS";
		case -1: return "CL_DEVICE_NOT_FOUND";
		case -2: return "CL_DEVICE_NOT_AVAILABLE";
		case -3: return "CL_COMPILER_NOT_AVAILABLE";
		case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
		case -5: return "CL_OUT_OF_RESOURCES";
		case -6: return "CL_OUT_OF_HOST_MEMORY";
		case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
		case -8: return "CL_MEM_COPY_OVERLAP";
		case -9: return "CL_IMAGE_FORMAT_MISMATCH";
		case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
		case -11: return "CL_BUILD_PROGRAM_FAILURE";
		case -12: return "CL_MAP_FAILURE";
		case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
		case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
		case -15: return "CL_COMPILE_PROGRAM_FAILURE";
		case -16: return "CL_LINKER_NOT_AVAILABLE";
		case -17: return "CL_LINK_PROGRAM_FAILURE";
		case -18: return "CL_DEVICE_PARTITION_FAILED";
		case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

			// compile-time errors
		case -30: return "CL_INVALID_VALUE";
		case -31: return "CL_INVALID_DEVICE_TYPE";
		case -32: return "CL_INVALID_PLATFORM";
		case -33: return "CL_INVALID_DEVICE";
		case -34: return "CL_INVALID_CONTEXT";
		case -35: return "CL_INVALID_QUEUE_PROPERTIES";
		case -36: return "CL_INVALID_COMMAND_QUEUE";
		case -37: return "CL_INVALID_HOST_PTR";
		case -38: return "CL_INVALID_MEM_OBJECT";
		case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
		case -40: return "CL_INVALID_IMAGE_SIZE";
		case -41: return "CL_INVALID_SAMPLER";
		case -42: return "CL_INVALID_BINARY";
		case -43: return "CL_INVALID_BUILD_OPTIONS";
		case -44: return "CL_INVALID_PROGRAM";
		case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
		case -46: return "CL_INVALID_KERNEL_NAME";
		case -47: return "CL_INVALID_KERNEL_DEFINITION";
		case -48: return "CL_INVALID_KERNEL";
		case -49: return "CL_INVALID_ARG_INDEX";
		case -50: return "CL_INVALID_ARG_VALUE";
		case -51: return "CL_INVALID_ARG_SIZE";
		case -52: return "CL_INVALID_KERNEL_ARGS";
		case -53: return "CL_INVALID_WORK_DIMENSION";
		case -54: return "CL_INVALID_WORK_GROUP_SIZE";
		case -55: return "CL_INVALID_WORK_ITEM_SIZE";
		case -56: return "CL_INVALID_GLOBAL_OFFSET";
		case -57: return "CL_INVALID_EVENT_WAIT_LIST";
		case -58: return "CL_INVALID_EVENT";
		case -59: return "CL_INVALID_OPERATION";
		case -60: return "CL_INVALID_GL_OBJECT";
		case -61: return "CL_INVALID_BUFFER_SIZE";
		case -62: return "CL_INVALID_MIP_LEVEL";
		case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
		case -64: return "CL_INVALID_PROPERTY";
		case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
		case -66: return "CL_INVALID_COMPILER_OPTIONS";
		case -67: return "CL_INVALID_LINKER_OPTIONS";
		case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

			// extension errors
		case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
		case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
		case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
		case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
		case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
		case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
		default: return "Unknown OpenCL error";
		}
	}

	cl_int handleError(cl_int err)
	{
		if (err != CL_SUCCESS)
		{
			std::cout << getErrorString(err) << std::endl;
			throw std::runtime_error(getErrorString(err));
		}
		else
		{
			return err;
		}
	}

	class StringInformation
	{
	public:
		char * string = NULL;
		StringInformation(int num_)
		{
			string = new char[num_];
			for (int i = 0; i < num_; i++)
			{
				string[i] = ' ';
			}
		}

		void writeString(const char* c)
		{
			if (string != NULL)
				delete[] string;
			int l = strlen(c);

			string = new char[l + 1];
			strcpy_s(string, l + 1, c);
		}

		char * readString()
		{
			return string;
		}

		~StringInformation()
		{
			if (string != NULL)
			{
				delete[] string;
			}

		}
	};


	static int sizeOf_[20]{ sizeof(cl_float),sizeof(cl_double),sizeof(cl_long),sizeof(cl_int), sizeof(cl_uint),sizeof(cl_char),sizeof(cl_half),7,8,9,10,11,12,13,14,15,16,17,18,19 };

	class ClArr
	{
	private:
	public:
		static const int ARR_FLOAT = 0;
		static const int ARR_DOUBLE = 1;
		static const int ARR_LONG = 2;
		static const int ARR_INT = 3;
		static const int ARR_UINT = 4;
		static const int ARR_BYTE = 5;
		static const int ARR_CHAR = 6;

		char * pArr;
		char * pAArr;
		int length;
		int arrType;
		ClArr(int n, int alignment, int arrType_)
		{
			arrType = arrType_;
			pArr = new (std::nothrow)char[n*sizeOf_[arrType] + alignment];
			if (pArr == nullptr)
			{
				std::cout << "C++ memory allocation failure. " << std::endl;
				throw std::runtime_error("C++ memory allocation failure. ");
			}

			length = n;
			if (((size_t)pArr) % alignment != 0)
			{
				pAArr = (char *)((alignment - ((size_t)pArr) % alignment) + ((size_t)pArr));
			}
			else
				pAArr = pArr;
		}

		~ClArr()
		{
			delete[]pArr;
			pArr = NULL;
			pAArr = NULL;
		}

	};

	__declspec(dllexport)
		void sizeOf(int * arr)
	{
		// C# byte = C99 char
		arr[ClArr::ARR_BYTE] = sizeof(cl_char);
		arr[ClArr::ARR_CHAR] = sizeof(cl_half);
		arr[ClArr::ARR_DOUBLE] = sizeof(cl_double);
		arr[ClArr::ARR_FLOAT] = sizeof(cl_float);
		arr[ClArr::ARR_INT] = sizeof(cl_int);
		arr[ClArr::ARR_LONG] = sizeof(cl_long);
		arr[ClArr::ARR_UINT] = sizeof(cl_uint);
	}

	__declspec(dllexport)
		ClArr * createArray(int n, int alignment, int arrType)
	{
		return new ClArr(n, alignment, arrType);
	}

	__declspec(dllexport)
		char * alignedArrHead(ClArr * hArr)
	{
		return hArr->pAArr;
	}

	__declspec(dllexport)
		void deleteArray(ClArr * hArr)
	{
		delete hArr;
	}





	class PlatformDeviceInformation
	{
	public:
		cl::Platform platform;
		std::vector<cl::Device> devicesCPU;
		std::vector<cl::Device> devicesGPU;
		std::vector<cl::Device> devicesACC;
		static const int CPU_CODE = CL_DEVICE_TYPE_CPU;
		static const int GPU_CODE = CL_DEVICE_TYPE_GPU;
		static const int ACC_CODE = CL_DEVICE_TYPE_ACCELERATOR;
		// 0=amd, 1=nvidia, 2=intel, 3=altera, 4=xilinx
		int vendor = -1;
		StringInformation *nameStr = nullptr;
		StringInformation *vendorStr = nullptr;
		PlatformDeviceInformation(cl::Platform p)
		{
			platform = p;
			
			devicesCPU = std::vector<cl::Device>();
			devicesGPU = std::vector<cl::Device>();
			devicesACC = std::vector<cl::Device>();
			std::vector<cl::Device> devicesCPUtmp;
			std::vector<cl::Device> devicesGPUtmp;
			std::vector<cl::Device> devicesACCtmp;
			cl_int err;
			err = (p.getDevices(CL_DEVICE_TYPE_CPU, &devicesCPUtmp));
			err = (p.getDevices(CL_DEVICE_TYPE_GPU, &devicesGPUtmp));
			err = (p.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devicesACCtmp));
			
	
			cl::string nameString;
			cl::string vendorString;
			err = handleError(p.getInfo(CL_PLATFORM_NAME, &nameString));
			err = handleError(p.getInfo(CL_PLATFORM_VENDOR, &vendorString));
		
			nameStr = new StringInformation(nameString.size());
			vendorStr = new StringInformation(vendorString.size());
			nameStr->writeString(nameString.data());
			vendorStr->writeString(vendorString.data());


			int cpuN = devicesCPUtmp.size();
			int gpuN = devicesGPUtmp.size();
			int accN = devicesACCtmp.size();
			for (int i = 0; i < cpuN; i++)
			{
				char majorVer = devicesCPUtmp[i].getInfo<CL_DEVICE_OPENCL_C_VERSION>().at(9);
				
				if ((majorVer >= '2'))
				{
					devicesCPU.push_back(devicesCPUtmp[i]);
				}
			}

			for (int i = 0; i < gpuN; i++)
			{
				char majorVer = devicesGPUtmp[i].getInfo<CL_DEVICE_OPENCL_C_VERSION>().at(9);
				
				if ((majorVer >= '2'))
				{
					devicesGPU.push_back(devicesGPUtmp[i]);
				}
			}

			for (int i = 0; i < accN; i++)
			{
				char majorVer = devicesACCtmp[i].getInfo<CL_DEVICE_OPENCL_C_VERSION>().at(9);
				
				if ((majorVer >= '2'))
				{
					devicesACC.push_back(devicesACCtmp[i]);
				}
			}

		}

		~PlatformDeviceInformation()
		{
			if (nameStr != nullptr)
			{
				delete nameStr;
				nameStr = nullptr;
			}

			if (vendorStr != nullptr)
			{
				delete vendorStr;
				vendorStr = nullptr;
			}
		}



		int numberOfCpus()
		{
			return devicesCPU.size();
		}
		int numberOfGpus()
		{
			return devicesGPU.size();
		}
		int numberOfAccelerators()
		{
			return devicesACC.size();
		}

	};

	class OpenClPlatformList
	{
	public:
		std::vector<cl::Platform> platforms;
		int numberOfPlatforms;

		OpenClPlatformList()
		{
			std::vector<cl::Platform> platformsTmp = std::vector< cl::Platform>();
			platforms = std::vector< cl::Platform>();
			handleError(cl::Platform::get(&platformsTmp));
			for (int i = 0; i < platformsTmp.size(); i++)
			{
				//CL_PLATFORM_VERSION(2.x) numpad 1 , keypad 1 possible bug with different chars
				if (platformsTmp[i].getInfo<CL_PLATFORM_VERSION>().at(7) == '2' || platformsTmp[i].getInfo<CL_PLATFORM_VERSION>().at(7) == '2')
				{
					bool adding = false;
					if (platformsTmp[i].getInfo<CL_PLATFORM_VERSION>().at(9) == '0' || platformsTmp[i].getInfo<CL_PLATFORM_VERSION>().at(9) == '0')
					{
						adding = true;
					}
					else if (platformsTmp[i].getInfo<CL_PLATFORM_VERSION>().at(9) == '1' || platformsTmp[i].getInfo<CL_PLATFORM_VERSION>().at(9) == '1')
					{
						adding = true;
					}
					else if (platformsTmp[i].getInfo<CL_PLATFORM_VERSION>().at(9) == '2' || platformsTmp[i].getInfo<CL_PLATFORM_VERSION>().at(9) == '2')
					{
						adding = true;
					}
					else if (platformsTmp[i].getInfo<CL_PLATFORM_VERSION>().at(9) == '3' || platformsTmp[i].getInfo<CL_PLATFORM_VERSION>().at(9) == '3')
					{
						adding = true;
					}

					// check if not "experimental" nor "beta"
					cl::string strPName = platformsTmp[i].getInfo<CL_PLATFORM_NAME>();
					if ((strPName.find("experimental") != cl::string::npos) ||
						(strPName.find("EXPERIMENTAL") != cl::string::npos) ||
						(strPName.find("experımental") != cl::string::npos) ||
						(strPName.find("Experimental") != cl::string::npos) ||
						(strPName.find("Experımental") != cl::string::npos))
					{
						adding = false;
					}

					if (adding)
						platforms.push_back(platformsTmp[i]);
				}

			}
			numberOfPlatforms = platforms.size();
		}


	};


	__declspec(dllexport)
		OpenClPlatformList * platformList()
	{
		return new OpenClPlatformList();
	}

	__declspec(dllexport)
		int numberOfPlatforms(OpenClPlatformList * hList)
	{
		return hList->numberOfPlatforms;
	}


	__declspec(dllexport)
		void deletePlatformList(OpenClPlatformList * p)
	{
		if (p != NULL)
			delete p;
		p = NULL;
	}





	__declspec(dllexport)
		PlatformDeviceInformation * createPlatform(OpenClPlatformList * p, int index)
	{
		return new PlatformDeviceInformation(p->platforms[index]);
	}

	__declspec(dllexport)
		void deletePlatform(PlatformDeviceInformation * hPlatform)
	{
		if (hPlatform != NULL)
			delete hPlatform;
	}



	class OpenClDevice
	{
	private:

	public:
		cl::Device clDevice;
		cl::Device clSubDevice;
		bool GDDR;
		int openclMajorVer = 0;
		int openclMinorVer = 0;
		int numberOfComputeUnits = 0;
		unsigned long memSize = 0;
		OpenClDevice(cl::Device device_)
		{
			clDevice = device_;
			cl_bool tmp = 0;
			// opencl 1.2:  CL_DEVICE_HOST_UNIFIED_MEMORY !!
			// opencl 2.0 - 2.x:  CL_DEVICE_SVM_CAPABILITIES !!
			handleError(clDevice.getInfo(CL_DEVICE_HOST_UNIFIED_MEMORY, &tmp));
			if (tmp == CL_TRUE)
				GDDR = false;
			else if (tmp == CL_FALSE)
				GDDR = true;

			cl_uint computeUnits;
			handleError(clDevice.getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &computeUnits));
			numberOfComputeUnits = computeUnits;
			cl_ulong memSizeTmp;
			handleError(clDevice.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &memSizeTmp));
			memSize = memSizeTmp;

		}

		int partition(int count_)
		{
			cl_device_partition_property p[]{ CL_DEVICE_PARTITION_BY_COUNTS, count_, CL_DEVICE_PARTITION_BY_COUNTS_LIST_END, 0 };
			std::vector<cl::Device> clDevices;
			cl_int err_create;
			if ((err_create = clDevice.createSubDevices(p, &clDevices)) == CL_SUCCESS)
			{
				clSubDevice = clDevices[0];
				clDevice = clDevices[0];
				numberOfComputeUnits = count_;
				return 0;
			}
			else
			{
				handleError(err_create);
			}
			return 1;
		}

		~OpenClDevice()
		{

		}
	};

	__declspec (dllexport)
		bool deviceGDDR(OpenClDevice * hDevice)
	{
		return hDevice->GDDR;
	}

	__declspec(dllexport)
		int deviceComputeUnits(OpenClDevice * hDevice)
	{
		return hDevice->numberOfComputeUnits;
	}

	__declspec(dllexport)
		unsigned long deviceMemSize(OpenClDevice * hDevice)
	{
		return hDevice->memSize;
	}

	class OpenClContext
	{
	private:

		cl_context_properties * ccp;

	public:
		cl::Context context;
		OpenClContext(cl::Device clDevice, cl::Platform platform)
		{
			ccp = new cl_context_properties[3];

			ccp[0] = CL_CONTEXT_PLATFORM;
			ccp[1] = (cl_context_properties)(platform.operator())();
			ccp[2] = 0;
			context = cl::Context(clDevice, ccp, NULL, NULL, NULL);

		}

		~OpenClContext()
		{
			if (ccp != NULL)
				delete[] ccp;

		}
	};


	__declspec(dllexport)
		OpenClContext * createContext(PlatformDeviceInformation * hPlatform, OpenClDevice * hDevice)
	{
		return new OpenClContext(hDevice->clDevice, hPlatform->platform);
	}

	__declspec(dllexport)
		int CODE_CPU()
	{
		return PlatformDeviceInformation::CPU_CODE;
	}

	__declspec(dllexport)
		int CODE_GPU()
	{
		return PlatformDeviceInformation::GPU_CODE;
	}

	__declspec(dllexport)
		int CODE_ACC()
	{
		return PlatformDeviceInformation::ACC_CODE;
	}


	__declspec(dllexport)
		int numberOfCpusInPlatform(PlatformDeviceInformation * hPlatform)
	{
		return hPlatform->devicesCPU.size();
	}

	__declspec(dllexport)
		int numberOfGpusInPlatform(PlatformDeviceInformation * hPlatform)
	{
		return hPlatform->devicesGPU.size();
	}

	__declspec(dllexport)
		int numberOfAcceleratorsInPlatform(PlatformDeviceInformation * hPlatform)
	{
		return hPlatform->devicesACC.size();
	}

	__declspec(dllexport)
		OpenClDevice * createDevice(PlatformDeviceInformation * hPlatform, int clDeviceType_, int index_)
	{
		if (clDeviceType_ == PlatformDeviceInformation::CPU_CODE)
			return new OpenClDevice(hPlatform->devicesCPU[index_]);
		else if (clDeviceType_ == PlatformDeviceInformation::GPU_CODE)
			return new OpenClDevice(hPlatform->devicesGPU[index_]);
		else if (clDeviceType_ == PlatformDeviceInformation::ACC_CODE)
			return new OpenClDevice(hPlatform->devicesACC[index_]);
	}

	__declspec(dllexport)
		OpenClDevice * createDeviceAsPartition(PlatformDeviceInformation * hPlatform, int clDeviceType_, int index_, int num_)
	{
		if (clDeviceType_ == PlatformDeviceInformation::CPU_CODE)
		{
			OpenClDevice *cld = new OpenClDevice(hPlatform->devicesCPU[index_]);
			cld->partition(num_);
			return cld;
		}
		else if (clDeviceType_ == PlatformDeviceInformation::GPU_CODE)
			return new OpenClDevice(hPlatform->devicesGPU[index_]);
		else if (clDeviceType_ == PlatformDeviceInformation::ACC_CODE)
			return new OpenClDevice(hPlatform->devicesACC[index_]);
	}


	__declspec(dllexport)
		StringInformation *  getPlatformInfo(PlatformDeviceInformation  *hPlatform)
	{
		StringInformation *sb = new StringInformation(1);
		sb->writeString(("{ accSayisi:" + std::to_string(hPlatform->numberOfAccelerators()) +
			",gpuSayisi:" + std::to_string(hPlatform->numberOfGpus()) +
			",cpuSayisi:" + std::to_string(hPlatform->numberOfCpus()) + "}").data());
		return sb;
	}


	__declspec(dllexport)
		void deleteContext(OpenClContext * context)
	{
		delete context;
	}



	__declspec(dllexport)
		void deleteDevice(OpenClDevice * hDevice)
	{
		delete hDevice;
	}


	class PlatformInfo
	{
	public:
		int numCPU;
		int numGPU;
		int numACC;
	};



	__declspec(dllexport)
		StringInformation * createString()
	{
		return new StringInformation(1);
	}

	__declspec(dllexport)
		void deleteString(StringInformation * hString)
	{
		delete hString;
	}

	__declspec(dllexport)
		void writeToString(StringInformation * hString, char * text_)
	{
		hString->writeString(std::string(text_).data());
	}

	__declspec(dllexport)
		char * readFromString(StringInformation * hStringInfo_)
	{
		return hStringInfo_->readString();
	}

	__declspec(dllexport)
		StringInformation * getPlatformNameString(PlatformDeviceInformation * hPlatform)
	{
		return hPlatform->nameStr;
	}

	__declspec(dllexport)
		StringInformation * getPlatformVendorNameString(PlatformDeviceInformation * hPlatform)
	{
		return hPlatform->vendorStr;
	}

	__declspec(dllexport)
		void jsonStringCallBack(StringInformation * hStringInfo_)
	{
		delete hStringInfo_;
		hStringInfo_ = NULL;
	}






	class OpenClCommandQueue
	{
	private:
		int counter;
		std::mutex m;

	public:
		cl::CommandQueue commandQueue;
		cl_command_queue device_queue=NULL; 
		OpenClCommandQueue(cl::Context context, cl::Device device, int async,bool defaultQueue=false)
		{
			counter = 0;
			cl_int err;
			if (defaultQueue)
			{
				
				cl_uint qsMax = device.getInfo<CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE>();
				cl_uint qsPref = device.getInfo<CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE>();
				//cl_uint qs=device.getInfo<CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE>();

				cl_queue_properties qprop[] = { CL_QUEUE_SIZE,(qsPref+qsMax)/2, CL_QUEUE_PROPERTIES, (cl_command_queue_properties)(CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_ON_DEVICE | CL_QUEUE_ON_DEVICE_DEFAULT | CL_QUEUE_PROFILING_ENABLE), 0 };
				device_queue = clCreateCommandQueueWithProperties(context.get(), device.get(), qprop, &err);
				//cl::DeviceQueueProperties dqp = cl::DeviceQueueProperties::Profiling;
				//commandQueue = cl::CommandQueue(context, device, CL_QUEUE_ON_DEVICE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_ON_DEVICE_DEFAULT, &err);
				//deviceCommandQueue = cl::DeviceCommandQueue(context, device, 1024*16, qprop, &err);
				
			}
			else
			{ 

				if (async != 0) 
					commandQueue = cl::CommandQueue(context, device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
				else
					commandQueue = cl::CommandQueue(context, device, cl::QueueProperties::Profiling, &err);
			}
			//commandQueue = cl::CommandQueue(context, device,0Ui64,&err); 
			//commandQueue = cl::CommandQueue(context, device,0Ui64,&err); 
			handleError(err);
		}

		// increments a counter whenever an observed marker calls its callback
		static void CL_CALLBACK markerCallbackCounter(cl_event evt, cl_int cint, void * vp)
		{
			((OpenClCommandQueue *)vp)->incrementMarkerCounter();
		}

		void incrementMarkerCounter()
		{
			m.lock();
			counter++;
			m.unlock();
		}

		// adds a marker to command queue to know when it has been reached(by a callback)
		void addMarkerForCountingWithCallback()
		{
			cl::Event evt;
			commandQueue.enqueueMarkerWithWaitList(NULL, &evt);
			evt.setCallback(CL_COMPLETE, &markerCallbackCounter, this);
		}

		void resetMarkerCallbackCounter()
		{
			m.lock();
			counter = 0;
			m.unlock();
		}

		int getCount()
		{
			int result = 0;
			m.lock();
			result = counter;
			m.unlock();
			return result;
		}


		~OpenClCommandQueue()
		{
			if (device_queue != NULL)
			{
				clReleaseCommandQueue(device_queue);
			}
		}
	};


	__declspec(dllexport)
		void addMarkerToCommandQueue(OpenClCommandQueue * hCommandQueue)
	{
		hCommandQueue->addMarkerForCountingWithCallback();
	}

	__declspec(dllexport)
		int getMarkerCounterOfCommandQueue(OpenClCommandQueue * hCommandQueue)
	{
		return hCommandQueue->getCount();
	}

	__declspec(dllexport)
		void resetMarkerCounterOfCommandQueue(OpenClCommandQueue * hCommandQueue)
	{
		hCommandQueue->resetMarkerCallbackCounter();
	}

	__declspec(dllexport)
		OpenClCommandQueue * createCommandQueue2(OpenClContext * hContext, OpenClDevice * hDevice, int async,bool defaultQueue)
	{
		return new OpenClCommandQueue(hContext->context, hDevice->clDevice, async,defaultQueue);
	}

	__declspec(dllexport)
		OpenClCommandQueue * createCommandQueue(OpenClContext * hContext, OpenClDevice * hDevice, int async)
	{
		return new OpenClCommandQueue(hContext->context, hDevice->clDevice, async);
	}

	__declspec(dllexport)
		void deleteCommandQueue(OpenClCommandQueue *hCommandQueue)
	{
		delete hCommandQueue;
	}


	class OpenClProgram
	{
	private:
	public:
		int err__ = -1;
		int err0__ = -1;
		StringInformation * errMsg__ = NULL;
		cl::Program program;
		OpenClProgram(cl::Context context, cl::Device device, std::string  string)
		{
			std::pair<const void *, size_t> stringValue = std::pair<const void *, size_t>();
			stringValue.first = string.data();
			stringValue.second = string.size();

			std::vector<std::pair<const void *, size_t>> strings__ = std::vector<std::pair<const void *, size_t>>();
			std::vector<cl::Device> clDevices__ = std::vector<cl::Device>();
			strings__.push_back(stringValue);
			clDevices__.push_back(device);

			program = cl::Program(context, string);

			err__ = program.build("-cl-std=CL2.0 -g -D CL_VERSION_2_0", 0, 0);


			size_t *logSize = new size_t[1];
			clGetProgramBuildInfo(program(), device(), CL_PROGRAM_BUILD_LOG, 0, NULL, logSize);
			char *logData = new char[(int)logSize[0]];
			clGetProgramBuildInfo(program(), device(), CL_PROGRAM_BUILD_LOG, logSize[0], logData, NULL);
			errMsg__ = new StringInformation((int)logSize[0]);
			writeToString(errMsg__, logData);
		}

		~OpenClProgram()
		{
		}
	};


	__declspec(dllexport)
		char * readProgramErrString(OpenClProgram * hProgram)
	{
		return hProgram->errMsg__->readString();
	}

	__declspec(dllexport)
		OpenClProgram * createProgram(OpenClContext * hContext, OpenClDevice * hDevice, StringInformation * hString)
	{
		return new OpenClProgram(hContext->context, hDevice->clDevice, std::string(hString->string));
	}

	__declspec(dllexport)
		int getProgramErr(OpenClProgram * hProgram)
	{
		return hProgram->err__;
	}


	__declspec(dllexport)
		void deleteProgram(OpenClProgram * hProgram)
	{
		delete hProgram;
	}

	class OpenClKernel
	{
	private:
	public:
		cl::Kernel kernel;
		cl_int err_;
		OpenClKernel(cl::Program program, const char * nameOfKernel_)
		{
			kernel = cl::Kernel(program, nameOfKernel_, &err_);

		}

		~OpenClKernel()
		{

		}

	};

	__declspec(dllexport)
		OpenClKernel * createKernel(OpenClProgram * hProgram, StringInformation * hString)
	{
		return new OpenClKernel(hProgram->program, hString->readString());
	}


	__declspec(dllexport)
		void deleteKernel(OpenClKernel * hKernel)
	{
		delete hKernel;
	}

	__declspec(dllexport)
		int getKernelErr(OpenClKernel * hKernel)
	{
		return hKernel->err_;
	}


	class OpenClInformation
	{
	public:
		int * clInformation__;// sizes of various types
		OpenClInformation()
		{
			clInformation__ = new int[10];
			clInformation__[0] = sizeof(cl_float);
			clInformation__[1] = sizeof(cl_double);
			clInformation__[2] = sizeof(cl_int);
			clInformation__[3] = sizeof(cl_long);
			clInformation__[4] = sizeof(cl_half);
			clInformation__[5] = sizeof(cl_char);
			clInformation__[6] = sizeof(cl_uint);
		}

		~OpenClInformation()
		{
			delete[] clInformation__;
		}
	};

	class OpenClBuffer
	{
	private:
	public:
		cl::Buffer buffer;
		OpenClInformation * ocl;
		int clb = 0;
		int es = 0;
		void * arr___ = NULL;
		bool gddr = false;
		bool readOnly_ = false;
		bool writeOnly_ = false;
		OpenClBuffer(cl::Context context, int numberOfElements_, int clInfo_, int isCSharpArray_, void * arr__, bool GDDR_BUFFER, bool readOnly, bool writeOnly)
		{
			readOnly_ = readOnly;
			writeOnly_ = writeOnly;
			gddr = GDDR_BUFFER;
			arr___ = arr__;
			ocl = new OpenClInformation();
			clb = clInfo_;
			es = numberOfElements_;
			cl_int err;
			/*
			CL_MEM_HOST_READ_ONLY--> "write_only"
			CL_MEM_WRITE_ONLY--> "write_only"

			CL_MEM_HOST_WRITE_ONLY--> "read_only"
			CL_MEM_READ_ONLY--> "read_only"
			*/
			if (readOnly)
			{
				if (GDDR_BUFFER)
					buffer = cl::Buffer(context, CL_MEM_HOST_WRITE_ONLY | CL_MEM_READ_ONLY, (size_t)(numberOfElements_*ocl->clInformation__[clInfo_]), NULL, &err);
				else if (isCSharpArray_ == 0 && arr__ != NULL)
				{
					buffer = cl::Buffer(context, CL_MEM_HOST_WRITE_ONLY | CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, (size_t)(numberOfElements_*ocl->clInformation__[clInfo_]), arr__, &err);
				}
				else
				{
					buffer = cl::Buffer(context, CL_MEM_HOST_WRITE_ONLY | CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, (size_t)(numberOfElements_*ocl->clInformation__[clInfo_]), NULL, &err);
				}
			}
			else if (writeOnly)
			{
				if (GDDR_BUFFER)
					buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, (size_t)(numberOfElements_*ocl->clInformation__[clInfo_]), NULL, &err);
				else if (isCSharpArray_ == 0 && arr__ != NULL)
				{
					buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_USE_HOST_PTR, (size_t)(numberOfElements_*ocl->clInformation__[clInfo_]), arr__, &err);
				}
				else
				{
					buffer = cl::Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, (size_t)(numberOfElements_*ocl->clInformation__[clInfo_]), NULL, &err);
				}
			}
			else
			{
				if (GDDR_BUFFER)
					buffer = cl::Buffer(context, CL_MEM_READ_WRITE, (size_t)(numberOfElements_*ocl->clInformation__[clInfo_]), NULL, &err);
				else if (isCSharpArray_ == 0 && arr__ != NULL)
				{
					buffer = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, (size_t)(numberOfElements_*ocl->clInformation__[clInfo_]), arr__, &err);
				}
				else
				{
					buffer = cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, (size_t)(numberOfElements_*ocl->clInformation__[clInfo_]), NULL, &err);
				}
			}

			handleError(err);
		}

		~OpenClBuffer()
		{
			delete ocl;
		}
	};


	__declspec(dllexport)
		OpenClBuffer * createBuffer(OpenClContext * hContext, int numElements_, int clInfo_, int isCSharpArray, void *arr_, bool GDDR_BUFFER, bool readOnly, bool writeOnly)
	{
		return new OpenClBuffer(hContext->context, numElements_, clInfo_, isCSharpArray, arr_, GDDR_BUFFER, readOnly, writeOnly);
	}


	__declspec(dllexport)
		void writeToBuffer(OpenClCommandQueue * hCommandQueue, OpenClBuffer * hBuffer, void * ptr)
	{
		cl_int err = 0;
		void * ptr2 = NULL;
		if (!hBuffer->gddr)
		{
			ptr2 = hCommandQueue->commandQueue.enqueueMapBuffer(hBuffer->buffer, false, (hBuffer->arr___ == NULL ? CL_MAP_WRITE_INVALIDATE_REGION : CL_MAP_WRITE), 0, hBuffer->es*hBuffer->ocl->clInformation__[hBuffer->clb], NULL, NULL, &err);
			handleError(err);
		}

		if (hBuffer->arr___ == NULL || hBuffer->gddr)
		{
			handleError(hCommandQueue->commandQueue.enqueueWriteBuffer(hBuffer->buffer, false, 0, hBuffer->es*hBuffer->ocl->clInformation__[hBuffer->clb], ptr, NULL, NULL));
		}
		if (!hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueUnmapMemObject(hBuffer->buffer, ptr2, NULL, NULL));
	}

	__declspec(dllexport)
		void readFromBuffer(OpenClCommandQueue * hCommandQueue, OpenClBuffer * hBuffer, void * ptr)
	{
		cl_int err;
		void * ptr2 = NULL;
		if (!hBuffer->gddr)
		{
			ptr2 = hCommandQueue->commandQueue.enqueueMapBuffer(hBuffer->buffer, false, CL_MAP_READ, 0, hBuffer->es*hBuffer->ocl->clInformation__[hBuffer->clb], NULL, NULL, &err);
			handleError(err);
		}
		if (hBuffer->arr___ == NULL || hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueReadBuffer(hBuffer->buffer, false, 0, hBuffer->es*hBuffer->ocl->clInformation__[hBuffer->clb], ptr, NULL, NULL));
		if (!hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueUnmapMemObject(hBuffer->buffer, ptr2, NULL, NULL));
	}

	// for C# side, char arrays must have  CharSet.Unicode in dllimport specifier
	__declspec(dllexport)
		void charArrayTest(void * ptr)
	{
		char*c = (char*)ptr;
		c[0] = 0;
		c[1] = 97;
		c[2] = 0;
		c[3] = 97;
		c[4] = 97;
		c[5] = 97;
		c[6] = 97;
		c[7] = 97;
		c[8] = 97;
		c[9] = 97;
		c[10] = 97;
		c[11] = 97;
		c[12] = 97;
		c[13] = 97;
		c[14] = 97;
	}


	__declspec(dllexport)
		void readFromBufferRanged(OpenClCommandQueue * hCommandQueue, OpenClBuffer * hBuffer, int reference_, int range_, void * ptr)
	{
		//cl::Event ev;
		size_t ref = reference_*hBuffer->ocl->clInformation__[hBuffer->clb];
		size_t m = range_*hBuffer->ocl->clInformation__[hBuffer->clb];
		char * p = (char *)ptr + ref;
		cl_int err;
		void * ptr2 = NULL;
		if (!hBuffer->gddr)
		{
			ptr2 = hCommandQueue->commandQueue.enqueueMapBuffer(hBuffer->buffer, false, CL_MAP_READ, ref, m, NULL, NULL, &err);
			handleError(err);
		}
		if (hBuffer->arr___ == NULL || hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueReadBuffer(hBuffer->buffer, false, ref, m, p, NULL, NULL));
		if (!hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueUnmapMemObject(hBuffer->buffer, ptr2, NULL, NULL));

	}

	__declspec(dllexport)
		void writeToBufferRanged(OpenClCommandQueue * hCommandQueue, OpenClBuffer * hBuffer, int reference_, int range_, void * ptr)
	{
		cl::Event ev;
		size_t ref = reference_*hBuffer->ocl->clInformation__[hBuffer->clb];
		size_t m = range_*hBuffer->ocl->clInformation__[hBuffer->clb];
		char * p = (char *)ptr + ref;
		cl_int err;
		void * ptr2 = NULL;
		if (!hBuffer->gddr)
		{
			ptr2 = hCommandQueue->commandQueue.enqueueMapBuffer(hBuffer->buffer, false, (hBuffer->arr___ == NULL ? CL_MAP_WRITE_INVALIDATE_REGION : CL_MAP_WRITE), ref, m, NULL, NULL, &err);
			handleError(err);
		}
		if (hBuffer->arr___ == NULL || hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueWriteBuffer(hBuffer->buffer, false, ref, m, p, NULL, &ev));
		if (!hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueUnmapMemObject(hBuffer->buffer, ptr2, NULL, NULL));
	}

	__declspec(dllexport)
		void deleteBuffer(OpenClBuffer * hBuffer)
	{
		delete hBuffer;
	}




	__declspec(dllexport)
		void setKernelArgument(OpenClKernel *hKernel, OpenClBuffer * hBuffer, int index_)
	{
		handleError(hKernel->kernel.setArg(index_, hBuffer->buffer));
	}



	class OpenClNDRange
	{
	private:
	public:
		size_t range;
		cl::NDRange ndrange;
		OpenClNDRange(int rng)
		{
			range = rng;
			ndrange = cl::NDRange(rng);
		}

		~OpenClNDRange()
		{

		}
	};

	__declspec(dllexport)
		OpenClNDRange * createNdRange(int n)
	{
		return new OpenClNDRange(n);
	}

	__declspec(dllexport)
		void deleteNdRange(OpenClNDRange * hRange)
	{
		delete hRange;
	}

	__declspec(dllexport)
		int compute(OpenClCommandQueue * hCommandQueue, OpenClKernel * hKernel, OpenClNDRange * hRangeReference_, OpenClNDRange * hRangeGlobal_, OpenClNDRange * hRangeLocal_)
	{
		int result_ = -1;
		//cl::Event ev;
		result_ = hCommandQueue->commandQueue.enqueueNDRangeKernel(hKernel->kernel, hRangeReference_->ndrange, hRangeGlobal_->ndrange, hRangeLocal_->ndrange, NULL, NULL /*&ev*/);
		return result_;
	}

	__declspec(dllexport)
		int computeRepeated(OpenClCommandQueue * hCommandQueue, OpenClKernel * hKernel, OpenClNDRange * hRangeReference_, OpenClNDRange * hRangeGlobal_, OpenClNDRange * hRangeLocal_, int repeats)
	{
		int result_ = -1;
		for (int i = 0; i<repeats; i++)
			hCommandQueue->commandQueue.enqueueNDRangeKernel(hKernel->kernel, hRangeReference_->ndrange, hRangeGlobal_->ndrange, hRangeLocal_->ndrange, NULL, NULL);
		return result_;
	}

	__declspec(dllexport)
		int computeRepeatedWithSyncKernel(OpenClCommandQueue * hCommandQueue, OpenClKernel * hKernel, OpenClNDRange * hRangeReference_, OpenClNDRange * hRangeGlobal_, OpenClNDRange * hRangeLocal_, int repeats, OpenClKernel * hSyncKernel, OpenClNDRange * hZeroRange)
	{
		int result_ = -1;
		for (int i = 0; i < repeats; i++)
		{
			hCommandQueue->commandQueue.enqueueNDRangeKernel(hKernel->kernel, hRangeReference_->ndrange, hRangeGlobal_->ndrange, hRangeLocal_->ndrange, NULL, NULL);
			hCommandQueue->commandQueue.enqueueNDRangeKernel(hSyncKernel->kernel, hZeroRange->ndrange, hRangeLocal_->ndrange, hRangeLocal_->ndrange, NULL, NULL);
		}
		return result_;
	}

	class OpenClEvent
	{
	private:
	public:
		cl::Event evt;
		OpenClEvent()
		{

		}

		~OpenClEvent()
		{


		}
	};

	class OpenClEventArray
	{
	private:
	public:
		std::vector<cl::Event> evt;
		int num__ = 0;
		bool isCopy = false;
		std::vector<bool> copies;
		OpenClEventArray(bool cpy_)
		{
			isCopy = cpy_;
		}

		void ekle(cl::Event e, bool cpy_)
		{
			evt.push_back(e);
			copies.push_back(cpy_);
			num__++;
		}

		~OpenClEventArray()
		{
			if (!isCopy)
			{
				for (int i = 0; i < num__; i++)
				{
					if (!copies[i])
						handleError(clReleaseEvent(evt[i].operator()()));
				}
			}
			evt.clear();
			copies.clear();
		}
	};


	__declspec(dllexport)
		OpenClEventArray * createEventArr(bool cpy_)
	{
		return new OpenClEventArray(cpy_);
	}

	__declspec(dllexport)
		void deleteEventArr(OpenClEventArray * hArr)
	{
		delete hArr;
	}

	__declspec(dllexport)
		void addToEventArr(OpenClEventArray * hArr, OpenClEvent *hEvt, bool cpy_)
	{
		hArr->ekle(hEvt->evt, cpy_);
	}

	__declspec(dllexport)
		void writeToBufferRangedEvent(OpenClCommandQueue * hCommandQueue, OpenClBuffer * hBuffer,
			int reference_, int range_, void * ptr, OpenClEventArray * hArr, OpenClEvent * hEvt)
	{
		size_t ref = reference_*hBuffer->ocl->clInformation__[hBuffer->clb];
		size_t m = range_*hBuffer->ocl->clInformation__[hBuffer->clb];
		char * p = (char *)ptr + ref;

		cl_int err;
		void * ptr2 = NULL;
		if (!hBuffer->gddr)
		{
			ptr2 = hCommandQueue->commandQueue.enqueueMapBuffer(hBuffer->buffer, false, (hBuffer->arr___ == NULL ? CL_MAP_WRITE_INVALIDATE_REGION : CL_MAP_WRITE), ref, m,
				(hArr->evt.size() > 0) ? &(hArr->evt) : NULL, NULL, &err);
			handleError(err);
		}
		// if gddr, no map-unmap, just read-write
		// if not gddr and if host ptr, just map-unmap
		// if not gddr and if not host ptr, map-unmap and read-write
		if (hBuffer->gddr || ((!hBuffer->gddr) && hBuffer->arr___ == NULL))
		{

			if (hBuffer->gddr)
				handleError(hCommandQueue->commandQueue.enqueueWriteBuffer(hBuffer->buffer, false, ref, m, p,
				(hArr->evt.size() > 0) ? &(hArr->evt) : NULL, hEvt == NULL ? NULL : &(hEvt->evt)));


			if (!hBuffer->gddr)
				handleError(hCommandQueue->commandQueue.enqueueWriteBuffer(hBuffer->buffer, false, ref, m, p,
					NULL, NULL));
		}
		if (!hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueUnmapMemObject(hBuffer->buffer, ptr2,
				NULL, hEvt == NULL ? NULL : &(hEvt->evt)));

	}

	__declspec(dllexport)
		int computeEvent(OpenClCommandQueue * hCommandQueue, OpenClKernel * hKernel,
			OpenClNDRange * hRangeReference_, OpenClNDRange * hRangeGlobal_,
			OpenClNDRange * hRangeLocal_, OpenClEventArray * hArr, OpenClEvent * hEvt)
	{
		int result_ = -1;

		result_ = handleError(hCommandQueue->commandQueue.enqueueNDRangeKernel(hKernel->kernel,
			hRangeReference_->ndrange,
			hRangeGlobal_->ndrange,
			hRangeLocal_->ndrange,
			(hArr->evt.size()>0) ? &(hArr->evt) : NULL, hEvt == NULL ? NULL : &(hEvt->evt)));

		return result_;
	}

	__declspec(dllexport)
		void readFromBufferRangedEvent(OpenClCommandQueue * hCommandQueue, OpenClBuffer * hBuffer,
			int reference_, int range_, void * ptr, OpenClEventArray * hArr, OpenClEvent * hEvt)
	{
		size_t ref = reference_*hBuffer->ocl->clInformation__[hBuffer->clb];
		size_t m = range_*hBuffer->ocl->clInformation__[hBuffer->clb];
		char * p = (char *)ptr + ref;

		cl_int err;
		void * ptr2 = NULL;
		if (!hBuffer->gddr)
		{
			ptr2 = hCommandQueue->commandQueue.enqueueMapBuffer(hBuffer->buffer, false, CL_MAP_READ, ref, m,
				(hArr->evt.size() > 0) ? &(hArr->evt) : NULL, NULL, &err);
			handleError(err);
		}
		// if gddr, no map-unmap, just read-write
		// if not gddr and if host ptr, just map-unmap
		// if not gddr and if not host ptr, map-unmap and read-write
		if (hBuffer->gddr || ((!hBuffer->gddr) && hBuffer->arr___ == NULL))
		{

			if (hBuffer->gddr)
				handleError(hCommandQueue->commandQueue.enqueueReadBuffer(hBuffer->buffer, false, ref, m, p,
				(hArr->evt.size() > 0) ? &(hArr->evt) : NULL, hEvt == NULL ? NULL : &(hEvt->evt)));


			if (!hBuffer->gddr)
				handleError(hCommandQueue->commandQueue.enqueueReadBuffer(hBuffer->buffer, false, ref, m, p,
					NULL, NULL));

		}
		if (!hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueUnmapMemObject(hBuffer->buffer, ptr2,
				NULL, hEvt == NULL ? NULL : &(hEvt->evt)));


	}

	__declspec(dllexport)
		void writeToBufferEvent(OpenClCommandQueue * hCommandQueue, OpenClBuffer * hBuffer,
			void * ptr, OpenClEventArray * hArr, OpenClEvent * hEvt)
	{
		cl_int err;
		void * ptr2 = NULL;
		if (!hBuffer->gddr)
		{
			ptr2 = hCommandQueue->commandQueue.enqueueMapBuffer(hBuffer->buffer, false, (hBuffer->arr___ == NULL ? CL_MAP_WRITE_INVALIDATE_REGION : CL_MAP_WRITE), 0, hBuffer->es*hBuffer->ocl->clInformation__[hBuffer->clb],
				&(hArr->evt), NULL, &err);
			handleError(err);
		}
		if (hBuffer->arr___ == NULL && !hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueWriteBuffer(hBuffer->buffer, false, 0,
				hBuffer->es*hBuffer->ocl->clInformation__[hBuffer->clb], ptr, NULL, NULL));
		else if (hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueWriteBuffer(hBuffer->buffer, false, 0,
				hBuffer->es*hBuffer->ocl->clInformation__[hBuffer->clb], ptr, &(hArr->evt), &(hEvt->evt)));


		if (!hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueUnmapMemObject(hBuffer->buffer, ptr2,
				NULL, &(hEvt->evt)));

	}

	__declspec(dllexport)
		void readFromBufferEvent(OpenClCommandQueue * hCommandQueue, OpenClBuffer * hBuffer,
			void * ptr, OpenClEventArray * hArr, OpenClEvent * hEvt)
	{
		cl_int err;
		void * ptr2 = NULL;
		if (!hBuffer->gddr)
		{
			ptr2 = hCommandQueue->commandQueue.enqueueMapBuffer(hBuffer->buffer, false, CL_MAP_READ, 0, hBuffer->es*hBuffer->ocl->clInformation__[hBuffer->clb],
				&(hArr->evt), NULL, &err);
			handleError(err);
		}
		if (hBuffer->arr___ == NULL && !hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueReadBuffer(hBuffer->buffer, false, 0,
				hBuffer->es*hBuffer->ocl->clInformation__[hBuffer->clb], ptr, NULL, NULL));
		else if (hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueReadBuffer(hBuffer->buffer, false, 0,
				hBuffer->es*hBuffer->ocl->clInformation__[hBuffer->clb], ptr, &(hArr->evt), &(hEvt->evt)));


		if (!hBuffer->gddr)
			handleError(hCommandQueue->commandQueue.enqueueUnmapMemObject(hBuffer->buffer, ptr2,
				NULL, &(hEvt->evt)));

	}

	__declspec(dllexport)
		OpenClEvent * createEvent()
	{
		return new OpenClEvent();
	}

	__declspec(dllexport)
		void deleteEvent(OpenClEvent * hEvt)
	{
		delete hEvt;
	}

	__declspec(dllexport)
		void finish(OpenClCommandQueue * hCommandQueue)
	{
		handleError(hCommandQueue->commandQueue.finish());
	}

	__declspec(dllexport)
		void flush(OpenClCommandQueue * hCommandQueue)
	{
		handleError(hCommandQueue->commandQueue.flush());
	}

	__declspec(dllexport)
		void wait2(OpenClCommandQueue * hCommandQueue, OpenClCommandQueue * hCommandQueue2)
	{
		std::vector<cl::Event>  evt;
		cl::Event evt0;
		cl::Event evt1;

		evt.push_back(evt0);
		evt.push_back(evt1);

		handleError(hCommandQueue->commandQueue.enqueueBarrierWithWaitList(NULL, &evt[0]));
		handleError(hCommandQueue2->commandQueue.enqueueBarrierWithWaitList(NULL, &evt[1]));

		cl_event * evt_ = new cl_event[2];
		evt_[0] = evt[0].operator()();
		evt_[1] = evt[1].operator()();

		clWaitForEvents(2, evt_);


		/*
		int evtStatus0 = 0;
		handleError(clGetEventInfo(evt_[0], CL_EVENT_COMMAND_EXECUTION_STATUS,
		sizeof(cl_int), &evtStatus0, NULL));

		while (evtStatus0 > 0)
		{

		handleError(clGetEventInfo(evt_[0], CL_EVENT_COMMAND_EXECUTION_STATUS,
		sizeof(cl_int), &evtStatus0, NULL));
		Sleep(0);
		}

		int evtStatus1 = 0;
		handleError(clGetEventInfo(evt_[1], CL_EVENT_COMMAND_EXECUTION_STATUS,
		sizeof(cl_int), &evtStatus1, NULL));

		while (evtStatus1 > 0)
		{

		handleError(clGetEventInfo(evt_[1], CL_EVENT_COMMAND_EXECUTION_STATUS,
		sizeof(cl_int), &evtStatus1, NULL));
		Sleep(0);
		}
		*/
		//clReleaseEvent(evt_[0]);
		//clReleaseEvent(evt_[1]);

		delete[] evt_;
	}



	class OpenClUserEvent
	{
	private:
	public:
		cl_event *evt;
		//std::vector<cl::Event> evt2;
		OpenClUserEvent(cl::Context ct)
		{
			evt = new cl_event[1];
			cl_int err___ = 0;
			evt[0] = clCreateUserEvent(ct.operator()(), &err___);

			if (err___ == CL_INVALID_CONTEXT)
			{
				printf("CL_INVALID_CONTEXT");
				throw std::runtime_error("invalid context");
			}
			if (err___ == CL_OUT_OF_RESOURCES)
			{
				printf("CL_OUT_OF_RESOURCES ");
				throw std::runtime_error("out of resources");
			}
			if (err___ == CL_OUT_OF_HOST_MEMORY)
			{
				printf("CL_OUT_OF_HOST_MEMORY ");
				throw std::runtime_error("out of host memory");
			}
			//evt2.push_back(cl::UserEvent(ct));
		}

		void decrementReference(cl::Context ct)
		{
			cl_int eventStatus = CL_QUEUED;
			cl_int error = 0;
			/*while (eventStatus != CL_COMPLETE)
			{
			error = clGetEventInfo(
			evt[0],
			CL_EVENT_COMMAND_EXECUTION_STATUS,
			sizeof(cl_int),
			&eventStatus,
			NULL);
			if (error != CL_SUCCESS)
			{
			printf("clGetEventInfo hata  = %d\n", error);

			}
			}*/

			cl_int err____ = clReleaseEvent(evt[0]);
			handleError(err____);
			if (err____ == CL_INVALID_CONTEXT)
			{
				printf("CL_INVALID_CONTEXT");
			}
			if (err____ == CL_OUT_OF_RESOURCES)
			{
				printf("CL_OUT_OF_RESOURCES ");
			}
			if (err____ == CL_OUT_OF_HOST_MEMORY)
			{
				printf("CL_OUT_OF_HOST_MEMORY ");
			}
			//cl::Event::waitForEvents(evt2);
		}

		void incrementReference()
		{
			clRetainEvent(evt[0]);
		}

		~OpenClUserEvent()
		{
			// clReleaseEvent(evt[0]);
			// gerektiði kadar release yapýlacak
			// yapýlacak: sadece gerekli olduðu zaman command queue 'ye eklenecek
			// çünkü burada fazladan release event gerekiyor. Sonra ref count negatif oluyor 0 olmalý
			//clReleaseEvent(evt[0]);
			//clReleaseEvent(evt[0]);
			//clReleaseEvent(evt[0]);
			//cl::Event::waitForEvents(evt2);
			//evt2.clear();
			delete[] evt;
		}
	};




	__declspec(dllexport)
		OpenClUserEvent * createUserEvent(OpenClContext  * hContext)
	{
		return new OpenClUserEvent(hContext->context);
	}

	__declspec(dllexport)
		void deleteUserEvent(OpenClUserEvent  * hEvent)
	{
		delete  hEvent;
	}

	__declspec(dllexport)
		void triggerUserEvent(OpenClUserEvent * hEvt)
	{
		clSetUserEventStatus(hEvt->evt[0], CL_COMPLETE);
	}

	__declspec(dllexport)
		void decrementUserEvent(OpenClUserEvent * hEvt, OpenClContext * hContext)
	{
		hEvt->decrementReference(hContext->context);
	}

	__declspec(dllexport)
		void incrementUserEvent(OpenClUserEvent * hEvt)
	{
		hEvt->incrementReference();
	}

	__declspec(dllexport)
		void addUserEvent(OpenClCommandQueue * hCommandQueue, OpenClUserEvent * hEvt)
	{
		cl_event ev;
		handleError(clEnqueueMarkerWithWaitList(hCommandQueue->commandQueue.operator()(), 1, hEvt->evt, &ev));
		handleError(clReleaseEvent(ev));
		//cl::Event ev;
		//hCommandQueue->commandQueue.enqueueMarkerWithWaitList(&(hEvt->evt2),&ev);


	}


	__declspec(dllexport)
		void waitN(OpenClCommandQueue ** hCommandQueueArray, OpenClCommandQueue * hCommandQueueToSync, const int n)
	{
		std::vector<cl::Event> evtVect;
		for (int i = 0; i < n; i++)
		{
			evtVect.push_back(cl::Event());
			//hCommandQueueArray[i]->commandQueue.enqueueBarrierWithWaitList(NULL, &evtVect[i]);
			hCommandQueueArray[i]->commandQueue.enqueueMarkerWithWaitList(NULL, &evtVect[i]);
			hCommandQueueArray[i]->commandQueue.flush();
		}
		//hCommandQueueToSync->commandQueue.enqueueBarrierWithWaitList(&evtVect, NULL);
		hCommandQueueToSync->commandQueue.enqueueMarkerWithWaitList(&evtVect, NULL);
		hCommandQueueToSync->commandQueue.finish();
	}

	__declspec(dllexport)
		void wait3(OpenClCommandQueue * hCommandQueue, OpenClCommandQueue * hCommandQueue2, OpenClCommandQueue * hCommandQueue3)
	{

		std::vector<cl::Event>  evt;
		cl::Event evt0;
		cl::Event evt1;
		cl::Event evt2;
		evt.push_back(evt0);
		evt.push_back(evt1);
		evt.push_back(evt2);
		handleError(hCommandQueue->commandQueue.enqueueBarrierWithWaitList(NULL, &evt[0]));
		handleError(hCommandQueue2->commandQueue.enqueueBarrierWithWaitList(NULL, &evt[1]));
		handleError(hCommandQueue3->commandQueue.enqueueBarrierWithWaitList(NULL, &evt[2]));
		cl_event * evt_ = new cl_event[3];
		evt_[0] = evt[0].operator()();
		evt_[1] = evt[1].operator()();
		evt_[2] = evt[2].operator()();
		clWaitForEvents(3, evt_);
		/*
		int evtStatus0 = 0;
		handleError(clGetEventInfo(evt_[0], CL_EVENT_COMMAND_EXECUTION_STATUS,
		sizeof(cl_int), &evtStatus0, NULL));

		while (evtStatus0 > 0)
		{

		handleError(clGetEventInfo(evt_[0], CL_EVENT_COMMAND_EXECUTION_STATUS,
		sizeof(cl_int), &evtStatus0, NULL));
		Sleep(0);
		}

		int evtStatus1 = 0;
		handleError(clGetEventInfo(evt_[1], CL_EVENT_COMMAND_EXECUTION_STATUS,
		sizeof(cl_int), &evtStatus1, NULL));

		while (evtStatus1 > 0)
		{

		handleError(clGetEventInfo(evt_[1], CL_EVENT_COMMAND_EXECUTION_STATUS,
		sizeof(cl_int), &evtStatus1, NULL));
		Sleep(0);
		}


		int evtStatus2 = 0;
		handleError(clGetEventInfo(evt_[2], CL_EVENT_COMMAND_EXECUTION_STATUS,
		sizeof(cl_int), &evtStatus2, NULL));

		while (evtStatus2 > 0)
		{

		handleError(clGetEventInfo(evt_[2], CL_EVENT_COMMAND_EXECUTION_STATUS,
		sizeof(cl_int), &evtStatus2, NULL));

		Sleep(0);
		}
		*/
		//clReleaseEvent(evt_[0]);
		//clReleaseEvent(evt_[1]);
		//clReleaseEvent(evt_[2]);
		delete[] evt_;
	}



	__declspec(dllexport)
		void getDeviceName(OpenClDevice * device, StringInformation * string)
	{
		cl::string str;
		handleError(device->clDevice.getInfo(CL_DEVICE_NAME, &str));
		string->writeString(str.c_str());
	}
	
	__declspec(dllexport)
		void getDeviceVendorName(OpenClDevice * device, StringInformation * string)
	{
		cl::string str;
		handleError(device->clDevice.getInfo(CL_DEVICE_VENDOR, &str)); 
		string->writeString(str.c_str());
	}


}

int main()
{
	std::cout << "deneme" << std::endl;
	OpenClPlatformList * pList = platformList();
	std::cout << numberOfPlatforms(pList) << std::endl;
	PlatformDeviceInformation * pd = createPlatform(pList, 0);
	std::cout << pd->nameStr->readString() << std::endl;
	getchar();
}





