/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkOptVectorLinearInterpolateImageFunction.h,v $
  Language:  C++
  Date:      $Date: 2009-10-29 11:19:19 $
  Version:   $Revision: 1.11 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkOptVectorLinearInterpolateImageFunction_h
#define __itkOptVectorLinearInterpolateImageFunction_h

#include "itkVectorInterpolateImageFunction.h"

namespace itk
{

/** \class LinearInterpolateImageFunction
 * \brief Linearly interpolate an image at specified positions.
 *
 * LinearInterpolateImageFunction linearly interpolates image intensity at
 * a non-integer pixel position. This class is templated
 * over the input image type and the coordinate representation type 
 * (e.g. float or double).
 *
 * This function works for N-dimensional images.
 *
 * \warning This function work only for images with scalar pixel
 * types. For vector images use VectorLinearInterpolateImageFunction.
 *
 * \sa VectorLinearInterpolateImageFunction
 *
 * \ingroup ImageFunctions ImageInterpolators 
 */
template <class TInputImage, class TCoordRep = double>
class ITK_EXPORT OptVectorLinearInterpolateImageFunction : 
  public VectorInterpolateImageFunction<TInputImage,TCoordRep> 
{
public:
  /** Standard class typedefs. */
  typedef OptVectorLinearInterpolateImageFunction                 Self;
  typedef VectorInterpolateImageFunction<TInputImage,TCoordRep>   Superclass;
  typedef SmartPointer<Self>                                      Pointer;
  typedef SmartPointer<const Self>                                ConstPointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(OptVectorLinearInterpolateImageFunction, VectorInterpolateImageFunction);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** OutputType typedef support. */
  typedef typename Superclass::OutputType OutputType;

  /** InputImageType typedef support. */
  typedef typename Superclass::InputImageType InputImageType;

  /** InputPixelType typedef support. */
  typedef typename Superclass::InputPixelType InputPixelType;

  /** RealType typedef support. */
  typedef typename Superclass::RealType RealType;

  /** Dimension underlying input image. */
  itkStaticConstMacro(ImageDimension, unsigned int,Superclass::ImageDimension);

  /** Index typedef support. */
  typedef typename Superclass::IndexType       IndexType;
  typedef typename Superclass::IndexValueType  IndexValueType;

  /** ContinuousIndex typedef support. */
  typedef typename Superclass::ContinuousIndexType ContinuousIndexType;

  /** Evaluate the function at a ContinuousIndex position
   *
   * Returns the linearly interpolated image intensity at a 
   * specified point position. No bounds checking is done.
   * The point is assume to lie within the image buffer.
   *
   * ImageFunction::IsInsideBuffer() can be used to check bounds before
   * calling the method. */
  virtual inline OutputType EvaluateAtContinuousIndex( const 
                                                         ContinuousIndexType &
                                                            index ) const
    {
    return this->EvaluateOptimized( Dispatch< ImageDimension >(), index );
    }

protected:
  OptVectorLinearInterpolateImageFunction();
  ~OptVectorLinearInterpolateImageFunction();
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  OptVectorLinearInterpolateImageFunction( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  /** Number of neighbors used in the interpolation */
  static const unsigned long  m_Neighbors;  

  struct DispatchBase {};
  template< unsigned int > struct Dispatch : DispatchBase {};

  inline OutputType EvaluateOptimized( const Dispatch<0> &,
                                       const ContinuousIndexType & index) const
    {
    return 0;
    }

  inline OutputType EvaluateOptimized( const Dispatch<1>&,
                                       const ContinuousIndexType & index) const
    {
    IndexType basei;

    basei[0] = Math::Floor<IndexValueType>(index[0]);
    if( basei[0] < this->m_StartIndex[0] )
      {
      basei[0] = this->m_StartIndex[0];
      }

    const double distance = index[0] - static_cast<double>(basei[0]);

    const InputPixelType val0 = this->GetInputImage()->GetPixel( basei );

    if(distance <= 0.)
      {
      return( static_cast<OutputType>( val0 ) );
      }

    ++basei[0];
    if(basei[0]>this->m_EndIndex[0])
      {
      return( static_cast<OutputType>( val0 ) );
      }
    const InputPixelType val1 = this->GetInputImage()->GetPixel( basei );

    return( static_cast<OutputType>( val0 + ( val1 - val0 ) * distance ) );
    }

  inline OutputType EvaluateOptimized( const Dispatch<2>&,
                                       const ContinuousIndexType & index) const
    {
    IndexType basei;

    basei[0] = Math::Floor<IndexValueType>(index[0]);
    if( basei[0] < this->m_StartIndex[0] )
      {
      basei[0] = this->m_StartIndex[0];
      }
    const double distance0 = index[0] - static_cast<double>(basei[0]);

    basei[1] = Math::Floor<IndexValueType>(index[1]);
    if( basei[1] < this->m_StartIndex[1] )
      {
      basei[1] = this->m_StartIndex[1];
      }
    const double distance1 = index[1] - static_cast<double>(basei[1]);


    const InputPixelType val00 = this->GetInputImage()->GetPixel( basei );
    if(distance0 <= 0. && distance1 <= 0.)
      {
      return( static_cast<OutputType>( val00 ) );
      }
    else if(distance1 <= 0.) // if they have the same "y"
      {
      ++basei[0];  // then interpolate across "x"
      if(basei[0]>this->m_EndIndex[0])
        {
        return( static_cast<OutputType>( val00 ) );
        }
      const InputPixelType val10 = this->GetInputImage()->GetPixel( basei );
      return( static_cast<OutputType>(val00 + (val10 - val00) * distance0) );
      }
    else if(distance0 <= 0.) // if they have the same "x"
      {
      ++basei[1];  // then interpolate across "y"
      if(basei[1]>this->m_EndIndex[1])
        {
        return( static_cast<OutputType>( val00 ) );
        }
      const InputPixelType val01 = this->GetInputImage()->GetPixel( basei );
      return( static_cast<OutputType>(val00 + (val01 - val00) * distance1) );
      }
    else // interpolate across "xy"
      {
      ++basei[0];
      if(basei[0]>this->m_EndIndex[0]) // interpolate across "y"
        {
        --basei[0];
        ++basei[1];
        if(basei[1]>this->m_EndIndex[1])
          {
          return( static_cast<OutputType>( val00 ) );
          }
        const InputPixelType val01 = this->GetInputImage()->GetPixel( basei );
        return( static_cast<OutputType>(val00 + (val01 - val00) * distance1) );
        }
      const InputPixelType val10 = this->GetInputImage()->GetPixel( basei );

      const InputPixelType valx0 = val00 + (val10 - val00) * distance0;
      
      ++basei[1];
      if(basei[1]>this->m_EndIndex[1]) // interpolate across "x"
        {
        return( static_cast<OutputType>( valx0 ) );
        }
      const InputPixelType val11 = this->GetInputImage()->GetPixel( basei );
      --basei[0];
      const InputPixelType val01 = this->GetInputImage()->GetPixel( basei );

      const InputPixelType valx1 = val01 + (val11 - val01) * distance0;

      return( static_cast<OutputType>( valx0 + (valx1-valx0) * distance1 ) );
      }
    }

  inline OutputType EvaluateOptimized( const Dispatch<3>&,
                                       const ContinuousIndexType & index) const
    {
    IndexType basei;

    basei[0] = Math::Floor<IndexValueType>(index[0]);
    if( basei[0] < this->m_StartIndex[0] )
      {
      basei[0] = this->m_StartIndex[0];
      }
    const double distance0 = index[0] - static_cast<double>(basei[0]);
 
    basei[1] = Math::Floor<IndexValueType>(index[1]);
    if( basei[1] < this->m_StartIndex[1] )
      {
      basei[1] = this->m_StartIndex[1];
      }
    const double distance1 = index[1] - static_cast<double>(basei[1]);
 
    basei[2] = Math::Floor<IndexValueType>(index[2]);
    if( basei[2] < this->m_StartIndex[2] )
      {
      basei[2] = this->m_StartIndex[2];
      }
    const double distance2 = index[2] - static_cast<double>(basei[2]);

    if(distance0<=0. && distance1<=0. && distance2<=0.)
      {
      return( static_cast<OutputType>( this->GetInputImage()->GetPixel( basei ) ) );
      }


    const InputPixelType val000 = this->GetInputImage()->GetPixel( basei );
    
    if(distance2 <= 0.)
      {
      if(distance1 <= 0.) // interpolate across "x"
        {
        ++basei[0];
        if(basei[0]>this->m_EndIndex[0])
          {
          return( static_cast<OutputType>( val000 ) );
          }
        const InputPixelType val100 = this->GetInputImage()->GetPixel( basei );
 
        return static_cast<OutputType>( val000 + (val100-val000) * distance0 );
        }
      else if(distance0 <= 0.) // interpolate across "y"
        {
        ++basei[1];
        if(basei[1]>this->m_EndIndex[1])
          {
          return( static_cast<OutputType>( val000 ) );
          }
        const InputPixelType val010 = this->GetInputImage()->GetPixel( basei );
 
        return static_cast<OutputType>( val000 + (val010-val000) * distance1 );
        }
      else  // interpolate across "xy"
        {
        ++basei[0];
        if(basei[0]>this->m_EndIndex[0]) // interpolate across "y"
          {
          --basei[0];
          ++basei[1];
          if(basei[1]>this->m_EndIndex[1])
            {
            return( static_cast<OutputType>( val000 ) );
            }
          const InputPixelType val010 = this->GetInputImage()->GetPixel( basei );

          return static_cast<OutputType>( val000 + (val010-val000) * distance1 );
          }
        const InputPixelType val100 = this->GetInputImage()->GetPixel( basei );

        const InputPixelType valx00 = val000 + (val100-val000) * distance0;

        ++basei[1];
        if(basei[1]>this->m_EndIndex[1]) // interpolate across "x"
          {
          return( static_cast<OutputType>( valx00 ) );
          }
        const InputPixelType val110 = this->GetInputImage()->GetPixel( basei );

        --basei[0];
        const InputPixelType val010 = this->GetInputImage()->GetPixel( basei );

        const InputPixelType valx10 = val010 + (val110-val010) * distance0;

        return static_cast<OutputType>( valx00 + (valx10-valx00) * distance1 );
        }
      }
    else
      {
      if(distance1 <= 0.)
        {
        if(distance0 <= 0.) // interpolate across "z"
          {
          ++basei[2];
          if(basei[2]>this->m_EndIndex[2])
            {
            return( static_cast<OutputType>( val000 ) );
            }
          const InputPixelType val001 = this->GetInputImage()->GetPixel( basei );
          
          return static_cast<OutputType>( val000 + (val001-val000) * distance2 );
          }
        else // interpolate across "xz"
          {
          ++basei[0];
          if(basei[0]>this->m_EndIndex[0]) // interpolate across "z"
            {
            --basei[0];
            ++basei[2];
            if(basei[2]>this->m_EndIndex[2])
              {
              return( static_cast<OutputType>( val000 ) );
              }
            const InputPixelType val001 = this->GetInputImage()->GetPixel( basei );

            return static_cast<OutputType>( val000 + (val001-val000) * distance2 );
            }
          const InputPixelType val100 = this->GetInputImage()->GetPixel( basei );

          const InputPixelType valx00 = val000 + (val100-val000) * distance0;

          ++basei[2];
          if(basei[2]>this->m_EndIndex[2]) // interpolate across "x"
            {
            return( static_cast<OutputType>( valx00 ) );
            }
          const InputPixelType val101 = this->GetInputImage()->GetPixel( basei );

          --basei[0];
          const InputPixelType val001 = this->GetInputImage()->GetPixel( basei );

          const InputPixelType valx01 = val001 + (val101-val001) * distance0;

          return static_cast<OutputType>( valx00 + (valx01-valx00) * distance2 );
          }
        }
      else if(distance0 <= 0.) // interpolate across "yz"
        {
        ++basei[1];
        if(basei[1]>this->m_EndIndex[1]) // interpolate across "z"
          {
          --basei[1];
          ++basei[2];
          if(basei[2]>this->m_EndIndex[2])
            {
            return( static_cast<OutputType>( val000 ) );
            }
          const InputPixelType val001 = this->GetInputImage()->GetPixel( basei );

          return static_cast<OutputType>( val000 + (val001-val000) * distance2 );
          }
        const InputPixelType val010 = this->GetInputImage()->GetPixel( basei );

        const InputPixelType val0x0 = val000 + (val010-val000) * distance1;

        ++basei[2];
        if(basei[2]>this->m_EndIndex[2]) // interpolate across "y"
          {
          return( static_cast<OutputType>( val0x0 ) );
          }
        const InputPixelType val011 = this->GetInputImage()->GetPixel( basei );

        --basei[1];
        const InputPixelType val001 = this->GetInputImage()->GetPixel( basei );

        const InputPixelType val0x1 = val001 + (val011-val001) * distance1;

        return static_cast<OutputType>( val0x0 + (val0x1-val0x0) * distance2 );
        }
      else // interpolate across "xyz"
        {
        ++basei[0];
        if(basei[0]>this->m_EndIndex[0]) // interpolate across "yz"
          {
          --basei[0];
          ++basei[1];
          if(basei[1]>this->m_EndIndex[1])  // interpolate across "z"
            {
            --basei[1];
            ++basei[2];
            if(basei[2]>this->m_EndIndex[2])
              {
              return( static_cast<OutputType>( val000 ) );
              }
            const InputPixelType val001 = this->GetInputImage()->GetPixel( basei );
            
            return static_cast<OutputType>( val000 + (val001-val000) * distance2 );
            }
          const InputPixelType val010 = this->GetInputImage()->GetPixel( basei );

          const InputPixelType val0x0 = val000 + (val010-val000) * distance1;

          ++basei[2];
          if(basei[2]>this->m_EndIndex[2]) // interpolate across "y"
            {
            return( static_cast<OutputType>( val0x0 ) );
            }
          const InputPixelType val011 = this->GetInputImage()->GetPixel( basei );

          --basei[1];
          const InputPixelType val001 = this->GetInputImage()->GetPixel( basei );

          const InputPixelType val0x1 = val001 + (val011-val001) * distance1;

          return static_cast<OutputType>( val0x0 + (val0x1-val0x0) * distance2 );
          }
        const InputPixelType val100 = this->GetInputImage()->GetPixel( basei );

        const InputPixelType valx00 = val000 + (val100-val000) * distance0;

        ++basei[1];
        if(basei[1]>this->m_EndIndex[1]) // interpolate across "xz"
          {
          --basei[1];
          ++basei[2];
          if(basei[2]>this->m_EndIndex[2]) // interpolate across "x"
            {
            return( static_cast<OutputType>( valx00 ) );
            }
          const InputPixelType val101 = this->GetInputImage()->GetPixel( basei );

          --basei[0];
          const InputPixelType val001 = this->GetInputImage()->GetPixel( basei );

          const InputPixelType valx01 = val001 + (val101-val001) * distance0;

          return static_cast<OutputType>( valx00 + (valx01-valx00) * distance2 );
          }
        const InputPixelType val110 = this->GetInputImage()->GetPixel( basei );

        --basei[0];
        const InputPixelType val010 = this->GetInputImage()->GetPixel( basei );

        const InputPixelType valx10 = val010 + (val110-val010) * distance0;

        const InputPixelType valxx0 = valx00 + (valx10-valx00) * distance1;

        
        ++basei[2];
        if(basei[2]>this->m_EndIndex[2]) // interpolate across "xy"
          {
          return( static_cast<OutputType>( valxx0 ) );
          }
        const InputPixelType val011 = this->GetInputImage()->GetPixel( basei );

        ++basei[0];
        const InputPixelType val111 = this->GetInputImage()->GetPixel( basei );

        --basei[1];
        const InputPixelType val101 = this->GetInputImage()->GetPixel( basei );

        --basei[0];
        const InputPixelType val001 = this->GetInputImage()->GetPixel( basei );

        const InputPixelType valx01 = val001 + (val101-val001) * distance0;
        const InputPixelType valx11 = val011 + (val111-val011) * distance0;
        const InputPixelType valxx1 = valx01 + (valx11-valx01) * distance1;

        return( static_cast<OutputType>( valxx0 + (valxx1-valxx0) * distance2 ) );
        }
      }
    }

  inline OutputType EvaluateOptimized( const DispatchBase &,
                                       const ContinuousIndexType & index) const
    {
    return this->EvaluateUnoptimized( index );
    }
                                       
  virtual inline OutputType EvaluateUnoptimized( 
                                       const ContinuousIndexType & index) const;
};

} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_OptVectorLinearInterpolateImageFunction(_, EXPORT, x, y) namespace itk { \
  _(2(class EXPORT OptVectorLinearInterpolateImageFunction< ITK_TEMPLATE_2 x >)) \
  namespace Templates { typedef OptVectorLinearInterpolateImageFunction< ITK_TEMPLATE_2 x > \
                                                  OptVectorLinearInterpolateImageFunction##y; } \
  }

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkOptVectorLinearInterpolateImageFunction+-.h"
#endif

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkOptVectorLinearInterpolateImageFunction.txx"
#endif

#endif
