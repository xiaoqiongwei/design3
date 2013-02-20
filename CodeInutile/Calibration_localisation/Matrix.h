#if !defined(AFX_MATRIX_H__64641E08_4E9A_11D4_BD19_00E0292D1826__INCLUDED_)
#define AFX_MATRIX_H__64641E08_4E9A_11D4_BD19_00E0292D1826__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MATRIX_ZERO_LIM      1e-100
#define MATRIX_NUM_PRECISION_DOUBLE 1.0e10 //The true precision is 1.0e15 but we
                                           //keep an error margin

class Matrix{

public:

   Matrix(int _nb_row=0, int _nb_col=0);
   Matrix(const Matrix &);
   virtual ~Matrix();

   int  SetSize(int _nb_row, int _nb_col);
   int  GetNbRow() const {return NbRow;}
   int  GetNbCol() const {return NbCol;}

   inline double &operator()(int _row, int _col) {return el[_row][_col];}
   inline double &operator()(int _row, int _col) const {return el[_row][_col];}

   double Det()  const;
   void   I();
   void   Transpose();
   double Invert();

   const Matrix &operator=(const Matrix &);
   const Matrix &operator*=(const Matrix &);
   const Matrix &operator*=(const double &);
   const Matrix &operator+=(const Matrix &);
   const Matrix &operator-=(const Matrix &);
   const Matrix &operator|=(const Matrix &);

   Matrix operator*(const Matrix &) const;
   Matrix operator*(const double &) const;
   Matrix operator+(const Matrix &) const;
   Matrix operator-(const Matrix &) const;
   Matrix operator|(const Matrix &) const; 
   int operator==(const Matrix &) const;
   
   void Print();

protected:

   int NbRow;
   int NbCol;

   double **el; //The pointer to the elements   
   
   // el
   // |
   // V
   // +->Row_1 : a11, ... a1j, ..., a1n
   // |  ...     ...      ...       ...
   // +->Row_i : ai1, ... aij, ..., ain
   // |  ...     ...      ...       ...
   // +->Row_m : am1, ... amj, ..., amn

   void   Equal(const Matrix &);

   double RowReduceToEchelonForm();
   double RowReduceToReducedEchelonForm();

};

#endif // !defined(AFX_MATRIX_H__64641E08_4E9A_11D4_BD19_00E0292D1826__INCLUDED_)
