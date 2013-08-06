#ifndef AirCppScriptSyntax_h__
#define AirCppScriptSyntax_h__

#include "AirCppScriptDefineType.h"

namespace	Air{
	namespace	CppScript{
		enum enumNodeType{
			enNT_Unknown,
			enNT_Global,
			enNT_NameSpace,
			enNT_Variable,
			enNT_Function,
			enNT_Object,
			enNT_Statement,
			enNT_Block,
		};
		enum	enumSyntaxError{
			enSE_OK,
			enSE_UnexpectedEnd,
			enSE_PreDeclare_Must_Fallow_A_CppKeyword,
			enSE_PreDeclare_Fallow_Unknown,
			enSE_Include_Must_Fallow_A_String,
			enSE_Define_Cant_Fallow_A_Number,
			enSE_Define_Cant_Fallow_A_String,
			enSE_Define_Cant_Fallow_A_CppKeyword,
			enSE_Define_Cant_Fallow_A_Operator,
			enSE_Define_Cant_Fallow_A_Delimiter,
			enSE_Unrecognized_Operator,
			enSE_Unrecognized_Delimiter,
			enSE_NameSpace_Cant_Fallow_A_Number,
			enSE_NameSpace_Cant_Fallow_A_String,
			enSE_NameSpace_Cant_Fallow_A_CppKeyword,
			enSE_NameSpace_Cant_Fallow_A_Operator,
			enSE_NameSpace_Cant_Fallow_A_Delimiter,
			enSE_Unsigned_Not_Match_Void,
			enSE_Unsigned_Not_Match_Bool,
			enSE_Unsigned_Not_Match_Float,
			enSE_Unsigned_Not_Match_Double,
			enSE_Unrecognized_Variable_Type,
			enSE_Miss_Variable_Name,
			enSE_Variable_Name_Cant_Be_A_Number,
			enSE_Variable_Name_Cant_Be_A_CppKeyword,
			enSE_Variable_Name_Cant_Be_A_String,
			enSE_Illegal_Variable_Name,
		};

		template<typename T>
		void	DeleteAllElement(T& t){
			T::iterator i = t.begin();
			for(;i!=t.end();i++){
				delete *i;
				*i=NULL;
			}
			t.clear();
		}

		void	Print(const char* str);
		class	Node;
		typedef std::list<Node*>	NodeList;
		class	Node{
		public:
			Node();
			virtual	~Node();

			virtual	enumSyntaxError		Parse(WordInfoVector& vecInfo,U32& idx);
			virtual	enumSyntaxError		ParsePreDeclare(WordInfoVector& vecInfo,U32& idx);
			virtual	enumSyntaxError		ParseInclude(WordInfoVector& vecInfo,U32& idx);
			virtual	enumSyntaxError		AddInclude(CAString& strName);
			virtual	enumSyntaxError		ParseDefine(WordInfoVector& vecInfo,U32& idx);
			virtual	enumSyntaxError		AddDefine(CAString& strName);
			virtual	enumSyntaxError		ParseCppKeyword(WordInfoVector& vecInfo,U32& idx);
			enumSyntaxError				IsVariableDeclare(WordInfoVector& vecInfo,U32& idx);
			enumSyntaxError				IsFunctionDeclare(WordInfoVector& vecInfo,U32& idx);
			enumSyntaxError				IsFunction(WordInfoVector& vecInfo,U32& idx);

			enumNodeType	GetType()const{return m_Type;};
			CAString&		GetName()const{return m_strName;};
			void			SetType(enumNodeType t){m_Type=t;};
			void			SetName(CAString& str){m_strName=str;};
			Node*			GetParent(){return m_pParent;};
			void			SetParent(Node* p){m_pParent=p;};
			U32				GetChildCount(){return m_lstChild.size();};
			void			AddChild(Node* p){
				if(p!=NULL){
					m_lstChild.push_back(p);
				}
			};
			Node*			GetRootNode();
		protected:
			enumNodeType	m_Type;
			AString			m_strName;
			NodeList		m_lstChild;
			Node*			m_pParent;
		};

		class	NameSpaceNode	:	public	Node{
		public:
			virtual	enumSyntaxError		Parse(WordInfoVector& vecInfo,U32& idx);
		};
		class	VariableNode	:	public	Node{
		public:
			VariableNode(){
				m_bHasInitValue	=	0;
			};
			virtual	enumSyntaxError		Parse(WordInfoVector& vecInfo,U32& idx);
			ObjectType					m_VariableType;
			AString						m_strInitValue;
			U32							m_bHasInitValue;
		};
		class	FunctionNode	:	public	Node{
		public:
			virtual	enumSyntaxError		Parse(WordInfoVector& vecInfo,U32& idx);
			ObjectType					m_ReturnType;
			ParameterVector				m_vecParameter;
		};
		class	ClassNode		:	public	Node{
		public:
			virtual	enumSyntaxError		Parse(WordInfoVector& vecInfo,U32& idx);
		};
	}
}
#endif // AirCppScriptSyntax_h__
