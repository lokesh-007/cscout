/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * The C symbol table
 *
 * $Id: stab.h,v 1.6 2001/09/21 14:14:19 dds Exp $
 */

#ifndef STAB_
#define STAB_

class Type;

// An C identifier as stored in the symbol table
class Id {
private:
	Token token;			// Any token representing the identifier
					// Needed for unification
	Type type;			// For type resolution
public:
	Id(const Token& tok, Type typ) :
		token(tok), type(typ) {}
	Id() : type(basic(b_undeclared)) {}			// Needed for map
	Type get_type() const { return type; }
	const Token& get_token() const { return token; }
	const string& get_name() const { return token.get_name(); }
};

typedef map<string,Id> Stab_element;

// A symbol table instance (used (two per block) for objects and tags)
class Stab {
private:
	Stab_element m;
public:
	Id const* lookup(const string& s) const;
	void define(const Token& tok, const Type& typ);
	void clear() { m.clear(); }
	Stab_element::const_iterator begin() const { return m.begin(); }
	Stab_element::const_iterator end() const { return m.end(); }
	static const string& get_name(const Stab_element::const_iterator x)
		{ return (*x).first; }
	static const Id& get_id(const Stab_element::const_iterator x)
		{ return (*x).second; }
	void merge_with(const Stab& m2)
		{ m.insert(m2.m.begin(), m2.m.end()); }
	friend ostream& operator<<(ostream& o,const Stab &s);
};

class Block;

typedef vector<Block> vectorBlock;

// Encapsulate symbols with function scope
// Per ANSI these are only the labels
class Function {
private:
	static Stab label;
public:
	// Called when entering or exiting a function
	// Either call will be enough
	static void enter() { label.clear(); }
	static void exit();
	friend void label_define(const Token& tok);
	friend void label_use(const Token& tok);
};

class Block {
private:
	static int current_block;	// Current block: >= 1
	static vectorBlock scope_block;
	Stab obj;
	Stab tag;

	static void define(Stab Block::*table, const Token& tok, const Type& t);
	static Id const * lookup(const Stab Block::*table, const string& name);
public:
	static const int lu_block = 0;	// Linkage unit definitions: 0
	static const int cu_block = 1;	// Compilation unit definitions: 1
	static void enter();
	static void exit();
	static int get_cur_block() { return current_block; }

	// Lookup and define of objects and struct/union/enum tags
	inline friend Id const * obj_lookup(const string& name);
	friend void obj_define(const Token& tok, Type t);
	inline friend Id const * tag_lookup(const string& name);
	friend void tag_define(const Token& tok, const Type& t);
};

inline Id const *
obj_lookup(const string& name)
{
	return Block::lookup(&(Block::obj), name);
}

inline Id const *
tag_lookup(const string& name)
{
	return Block::lookup(&(Block::tag), name);
}

#endif // STAB_
