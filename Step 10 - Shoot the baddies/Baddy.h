// Baddy.h: interface for the Baddy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BADDY_H__EE8043BC_BBC3_4977_B3A4_634EA30D8F13__INCLUDED_)
#define AFX_BADDY_H__EE8043BC_BBC3_4977_B3A4_634EA30D8F13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Baddy
{
public:
	Baddy();
	~Baddy();

public:
	bool Hit(int damage) { hitpoints -= damage; return (hitpoints <= 0)?false:true; }

public:
	double x;
	double y;
	int score;
	int hitpoints;
};

#endif // !defined(AFX_BADDY_H__EE8043BC_BBC3_4977_B3A4_634EA30D8F13__INCLUDED_)
