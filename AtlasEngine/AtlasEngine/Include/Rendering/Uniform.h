#pragma once

#include <GL\glew.h>
#include <cstdint>

// Forward declares for maths
template <typename T>
class TVector2;

template <typename T>
class TVector3;

template <typename T>
class TVector4;

struct FMatrix4;

namespace EZGL
{
	// GL wrapper for using a glUniform. 
	class FUniform
	{
	public:
		/**
		* Constructs a unbound uniform object.
		*/
		FUniform();

		/**
		* Constructs a uniform object that is set
		* to a specific uniform location on a given program.
		* @param ProgramID - The shader program for the uniform.
		* @param UniformName - The name of the uniform variable.
		*/
		FUniform(const GLuint ProgramID, const GLchar* UniformName);

		~FUniform() = default;

		/**
		* Binds this object to a new uniform variable.	
		* @param ProgramID - The shader program for the uniform.
		* @param UniformName - The name of the uniform variable.
		*/
		void Bind(const GLuint ProgramID, const GLchar* UniformName);

		/**
		* Retrieves the uniform location of the currently bound
		* uniform.
		*/
		GLint GetLocation() const;

		template <typename T>
		/**
		* Sets data to the currently bound uniform variable. 
		* Non primitive datatypes must specilize this template
		* function.
		* @param Data - The data to set.
		*/
		void SetUniform(T Data);
		
		template <typename T>
		void SetUniform(T Data1, T Data2);

		template <typename T>
		/**
		* Sets a vector to the currently bound uniform variable.
		* @param Count - The number of sets of data.
		* @param Data - The data to set.
		*/
		void SetVector(GLsizei Count, const T* Data);

		template <typename T>
		/**
		* Sets a matrix to the currently bound uniform variable.
		* @param Count - The number of sets of data.
		* @param Transpose - If this matrix should be transposed.
		* @param Data - The data to set.
		*/
		void SetMatrix(GLsizei Count, GLboolean Transpose, const T* Data);

	private:
		GLint mLocation;
	};

	inline FUniform::FUniform()
		: mLocation(-1)
	{}

	inline FUniform::FUniform(const GLuint ProgramID, const GLchar* UniformName)
		: mLocation(-1)
	{
		Bind(ProgramID, UniformName);
	}

	inline void FUniform::Bind(const GLuint ProgramID, const GLchar* UniformName)
	{
		mLocation = glGetUniformLocation(ProgramID, UniformName);
	}

	inline GLint FUniform::GetLocation() const
	{
		return mLocation;
	}

	template <>
	inline void FUniform::SetUniform<float>(float Data)
	{
		glUniform1f(mLocation, Data);
	}

	template <>
	inline void FUniform::SetUniform<float>(float Data1, float Data2)
	{
		glUniform2f(mLocation, Data1, Data2);
	}

	template <>
	inline void FUniform::SetUniform<int32_t>(int32_t Data)
	{
		glUniform1i(mLocation, Data);
	}

	template <>
	inline void FUniform::SetUniform<int32_t>(int32_t Data1, int32_t Data2)
	{
		glUniform2i(mLocation, Data1, Data2);
	}

	template <>
	inline void FUniform::SetUniform<uint32_t>(uint32_t Data)
	{
		glUniform1ui(mLocation, Data);
	}

	template <>
	inline void FUniform::SetUniform<uint32_t>(uint32_t Data1, uint32_t Data2)
	{
		glUniform2ui(mLocation, Data1, Data2);
	}

	template <>
	inline void FUniform::SetVector<TVector2<float>>(GLsizei Count, const TVector2<float>* Data)
	{
		glUniform2fv(mLocation, Count, (float*)Data);
	}

	template <>
	inline void FUniform::SetVector<TVector2<int32_t>>(GLsizei Count, const TVector2<int32_t>* Data)
	{
		glUniform2iv(mLocation, Count, (int32_t*)Data);
	}

	template <>
	inline void FUniform::SetVector<TVector2<uint32_t>>(GLsizei Count, const TVector2<uint32_t>* Data)
	{
		glUniform2uiv(mLocation, Count, (uint32_t*)Data);
	}

	template <>
	inline void FUniform::SetVector<TVector3<float>>(GLsizei Count, const TVector3<float>* Data)
	{
		glUniform3fv(mLocation, Count, (float*)Data);
	}

	template <>
	inline void FUniform::SetVector<TVector3<int32_t>>(GLsizei Count, const TVector3<int32_t>* Data)
	{
		glUniform3iv(mLocation, Count, (int32_t*)Data);
	}

	template <>
	inline void FUniform::SetVector<TVector3<uint32_t>>(GLsizei Count, const TVector3<uint32_t>* Data)
	{
		glUniform3uiv(mLocation, Count, (uint32_t*)Data);
	}

	template <>
	inline void FUniform::SetVector<TVector4<float>>(GLsizei Count, const TVector4<float>* Data)
	{
		glUniform4fv(mLocation, Count, (float*)Data);
	}

	template <>
	inline void FUniform::SetVector<TVector4<int32_t>>(GLsizei Count, const TVector4<int32_t>* Data)
	{
		glUniform4iv(mLocation, Count, (int32_t*)Data);
	}

	template <>
	inline void FUniform::SetVector<TVector4<uint32_t>>(GLsizei Count, const TVector4<uint32_t>* Data)
	{
		glUniform4uiv(mLocation, Count, (uint32_t*)Data);
	}

	template <>
	inline void FUniform::SetMatrix<FMatrix4>(GLsizei Count, GLboolean Transpose, const FMatrix4* Data)
	{
		glUniformMatrix4fv(mLocation, Count, Transpose, (float*)Data);
	}
}