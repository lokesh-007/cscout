/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * The type-system structure
 * See also type2.h for derived classes depending on Stab
 *
 * $Id: type.h,v 1.13 2001/09/21 14:14:19 dds Exp $
 */

#ifndef TYPE_
#define TYPE_

enum e_btype {
	b_abstract,		// Abstract declaration target, to be filled-in
	b_void, b_char, b_short, b_int, b_long, b_float, b_double, b_ldouble,
	b_undeclared,		// Undeclared object
	b_llong			// long long
};

enum e_sign {
	s_none,			// Neither signed, nor unsigned
	s_signed,
	s_unsigned
};

enum e_storage_class {
	c_unspecified,
	c_typedef,
	c_extern,
	c_static,
	c_auto,
	c_register
};


enum e_tagtype {tt_struct, tt_union, tt_enum};

class Id;
class Tbasic;
class Type;
class Stab;

class Type_node {
	friend class Type;
private:
	int use;				// Use count
	// This is also the place to store type qualifiers, because the can be
	// applied to any type.  Furtunatelly we can afford to ignore them.
protected:
	Type_node() : use(1) {}

	virtual Type subscript() const;		// Arrays and pointers
	virtual Type deref() const;		// Arrays and pointers
	virtual Type call() const;		// Function
	virtual Type type() const;		// Identifier
	virtual Type clone() const;		// Deep copy
	virtual Id const* member(const string& name) const;	// Structure and union
	virtual bool is_ptr() const { return false; }// True for ptr arithmetic types
	virtual bool is_valid() const { return true; }// False for undeclared
	virtual bool is_basic() const { return false; }// False for undeclared
	virtual bool is_abstract() const { return false; }	// True for abstract types
	virtual const string& get_name() const;	// True for identifiers
	virtual const Ctoken& get_token() const;// True for identifiers
	virtual void set_abstract(Type t);	// Set abstract basic type to t
	virtual void set_storage_class(Type t);	// Set typedef's underlying storage class to t
	virtual enum e_storage_class get_storage_class() const;// Return the declaration's storage class
	virtual void add_member(const Token &tok, const Type &typ);
	virtual Type get_default_specifier();
	virtual void merge_with(Type t);
	virtual const Stab& get_members() const;

	bool is_typedef() const { return get_storage_class() == c_typedef; }// True for typedefs
public:
	// For merging
	virtual Type merge(Tbasic *b);
	virtual Tbasic *tobasic();
	virtual void print(ostream &o) const = 0;
};

// Used by types with a storage class: Tbasic and Tsu
class Tstorage {
private:
	enum e_storage_class sclass;
public:
	Tstorage (enum e_storage_class sc) : sclass(sc) {}
	Tstorage() { sclass = c_unspecified; }
	enum e_storage_class get_storage_class() const {return sclass; }
	void set_storage_class(Type t);
};


// Basic type
class Tbasic: public Type_node {
private:
	enum e_btype type;
	enum e_sign sign;
	Tstorage sclass;
public:
	Tbasic(enum e_btype t = b_abstract, enum e_sign s = s_none,
		enum e_storage_class sc = c_unspecified) :
		type(t), sign(s), sclass(sc) {}
	Type clone() const;
	bool is_valid() const { return type != b_undeclared; }
	bool is_abstract() const { return type == b_abstract; }
	bool is_basic() const { return true; }// False for undeclared
	void print(ostream &o) const;
	Type merge(Tbasic *b);
	Tbasic *tobasic() { return this; }
	enum e_storage_class get_storage_class() const { return sclass.get_storage_class(); }
	inline void set_storage_class(Type t);
};

/*
 * Handle class for representing types.
 * It encapsulates the type node memory management.
 * See Koening & Moo: Ruminations on C++ Addison-Wesley 1996, chapter 8
 */
class Type {
private:
	Type_node *p;
public:
	Type(Type_node *n) : p(n) {}
	Type() { p = new Tbasic(b_undeclared); }
	// Creation functions
	friend Type basic(enum e_btype t = b_abstract, enum e_sign s = s_none,
			  enum e_storage_class sc = c_unspecified);
	friend Type array_of(Type t);
	friend Type pointer_to(Type t);
	friend Type function_returning(Type t);
	friend Type implict_function();
	friend Type enum_tag();
	friend Type struct_tag();
	friend Type union_tag();
	friend Type struct_union(const Token &tok, const Type &typ, const Type &spec);
	friend Type struct_union(const Type &spec);
	friend Type label();
	friend Type identifier(const Ctoken& c);
	// To print
	friend ostream& operator<<(ostream& o,const Type &t) { t.p->print(o); }

	// Add the declaration of an identifier to the symbol table
	void declare();

	// Manage use count of underlying Type_node
	Type(const Type& t) { p = t.p; ++p->use; }	// Copy
	~Type() { if (--p->use == 0) delete p; }
	Type& operator=(const Type& t);

	// Interface to the Type_node functionality
	Type clone() const 		{ return p->clone(); }
	Type subscript() const		{ return p->subscript(); }
	Type deref() const		{ return p->deref(); }
	Type call() const		{ return p->call(); }
	Type type() const		{ return p->type(); }
	void set_abstract(Type t)	{ return p->set_abstract(t); }
	void set_storage_class(Type t)	{ return p->set_storage_class(t); }
	bool is_ptr() const		{ return p->is_ptr(); }
	bool is_typedef() const		{ return p->is_typedef(); }
	bool is_valid() const		{ return p->is_valid(); }
	bool is_basic() const		{ return p->is_basic(); }
	bool is_abstract() const	{ return p->is_abstract(); }
	const string& get_name() const	{ return p->get_name(); }
	const Ctoken& get_token() const { return p->get_token(); }
	enum e_storage_class get_storage_class() const 
					{return p->get_storage_class(); }
	Type get_default_specifier() const 
					{ return p->get_default_specifier(); }
	void add_member(const Token &tok, const Type &typ)
					{ p->add_member(tok, typ); }
	void merge_with(Type t) { p->merge_with(t) ; }
	const Stab& get_members() const	{ return p->get_members(); }
	Id const* member(const string& name) const	// Structure and union
					{ return p->member(name); }
	friend Type merge(Type a, Type b) { return a.p->merge(b.p->tobasic()); }
};


inline void Tbasic::set_storage_class(Type t) { sclass.set_storage_class(t); }

/*
 * We can not use a union since its members have constructors and desctructors.
 * We thus define a structure with a single element to turn-on the yacc
 * type-checking mechanisms
 * Elements that have a type (which must be of type Type) are defined as <t>
 */
typedef struct {
	Type t;
} YYSTYPE;

#endif // TYPE_
