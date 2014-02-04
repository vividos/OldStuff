//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file Xml.hpp xml classes
//
#pragma once

// includes
#include <memory>
#include <vector>
#include <map>

/// \brief XML classes
namespace XML
{

// smart pointer typedefs

typedef std::shared_ptr<class XmlNode> XmlNodePtr;
typedef std::shared_ptr<class XmlNodeList> XmlNodeListPtr;
typedef std::shared_ptr<class XmlAttribute> XmlAttributePtr;
typedef std::shared_ptr<class XmlAttributeCollection> XmlAttributeCollectionPtr;
typedef std::shared_ptr<class XmlElement> XmlElementPtr;
typedef std::shared_ptr<class XmlText> XmlTextPtr;
typedef std::shared_ptr<class XmlComment> XmlCommentPtr;
typedef std::shared_ptr<class XmlCDataSection> XmlCDataSectionPtr;
typedef std::shared_ptr<class XmlDeclaration> XmlDeclarationPtr;
typedef std::shared_ptr<class XmlDocument> XmlDocumentPtr;
typedef std::shared_ptr<class XmlReader> XmlReaderPtr;
typedef std::shared_ptr<class XmlWriter> XmlWriterPtr;

//! xml node; base class for all node types
class XmlNode //: public std::enable_shared_from_this<XmlNode>
{
public:
   //! dtor
   ~XmlNode() throw(){}

   // types

   //! node type
   enum XmlNodeType
   {
      nodeTypeElement,        //!< element node
      nodeTypeText,           //!< text node
      nodeTypeCDataSection,   //!< CDATA section node "<!CDATA[[{1}]]>
      nodeTypeProcessingInstruction, //!< PI node "<?{0} {1}?>"
      nodeTypeComment,        //!< comment "<!--{1}-->
      nodeTypeXmlDeclaration, //!< <?xml version="1.0"?>
      nodeTypeDocument,       //!< document node
      //nodeTypeEntityReference,
      nodeTypeEndElement,     //!< end element </{0}>
      nodeTypeAttribute,      //!< attribute node
      nodeTypeInvalid         //!< invalid
   };

   // attributes

   //! returns list of child nodes
   XmlNodeListPtr ChildNodes(){ return m_spAllChildNodes; }

   //! returns list of child nodes; const-version
   const XmlNodeListPtr ChildNodes() const { return m_spAllChildNodes; }

   //! returns first child
   XmlNodePtr FirstChild();

   //! returns if node has child nodes
   bool HasChildNodes() const;

   //! returns item with given name
   XmlNodePtr Item(const CString& cszName);

   //! returns node name
   CString Name() const { return m_cszName; }
   //! sets node name
   void Name(const CString& cszName){ m_cszName = cszName; }

   //! returns next sibling
   XmlNodePtr NextSibling();

   //! returns node type
   XmlNodeType NodeType() const { return m_enNodeType; }

   //! returns owner document
   XmlDocumentPtr OwnerDocument(){ return m_spDocument; }

   //! returns parent node
   XmlNodePtr ParentNode();

   //! returns previous sibling
   XmlNodePtr PreviousSibling();

   //! returns node value
   CString Value() const { return m_cszValue; }
   //! sets node value
   void Value(const CString& cszValue){ m_cszValue = cszValue; }

   // methods

   //! appends child node to this node
   void AppendChild(XmlNodePtr spNode);

   //void CloneNode(bool bDeep);

   //! prepends child node to this node
   void PrependChild(XmlNodePtr spNode);

   //! removes all nodes
   virtual void RemoveAll();

   //! removes child
   void RemoveChild(XmlNodePtr spNode);

   //! replaces child with new node
   void ReplaceChild(XmlNodePtr spNewNode, XmlNodePtr spOldNodeToReplace);

   //! selects all nodes that matches the given expression
   XmlNodeListPtr SelectNodes(const CString cszXPathExpression);

   //! selects the first node that matches the expression
   XmlNodePtr SelectSingleNode(const CString cszXPathExpression);

   //! writes out node and all subnodes
   virtual void WriteTo(XmlWriterPtr spXmlWriter) const = 0;

   /// casts node to given template type; returns NULL pointer when node is not of that type
   template <typename T>
   std::shared_ptr<T> CastNode();
/*   {
      // when this method is called, then there's no specialisation for a known type, so
      // you cannot convert to this node type
      ATLASSERT(false);
   }
*/

   //template<> inline std::shared_ptr<class XmlDeclaration> CastNode();

protected:
   friend XmlDocument;

   /// ctor
   XmlNode(XmlDocumentPtr spDocument, XmlNodeType enNodeType)
      :m_spDocument(spDocument),
       m_enNodeType(enNodeType)
   {
   }

   /// returns document the node belongs to
   void Document(XmlDocumentPtr spDocument){ m_spDocument = spDocument; }

   /// returns parent node
   void Parent(XmlNodePtr spNode){ m_spParent = spNode; }

private:
   /// node name
   CString m_cszName;

   /// node value
   CString m_cszValue;

   /// node type
   XmlNodeType m_enNodeType;

   /// child nodes
   XmlNodeListPtr m_spAllChildNodes;

   /// document the node belongs to
   XmlDocumentPtr m_spDocument;

   /// parent node
   XmlNodePtr m_spParent;
};

//! xml attribute
class XmlAttribute: public XmlNode
{
public:
   //! dtor
   ~XmlAttribute() throw(){}

   // attributes

   //! returns element the attribute belongs to
   XmlElementPtr OwnerElement(){ return m_spOwnerElement; }

   // methods

   //! writes attribute using writer
   virtual void WriteTo(XmlWriterPtr spXmlWriter) const;

protected:
   friend XmlDocument;
   friend XmlElement;
   friend class XmlAttributeCollection;

   //! ctor; creates new attribute with given name
   XmlAttribute(const CString& cszName, XmlDocumentPtr spDocument)
      :XmlNode(spDocument, nodeTypeAttribute)
   {
      Name(cszName);
   }

   //! sets owner element
   void OwnerElement(XmlElementPtr spElement){ m_spOwnerElement = spElement; }

private:
   //! owner element
   XmlElementPtr m_spOwnerElement;
};

//! collection of xml attributes
class XmlAttributeCollection
{
public:
   // attributes

   //! returns attribute with given name
   XmlAttributePtr ItemOf(const CString& cszAttrName);

   //! returns attribute with given index
   XmlAttributePtr ItemOf(size_t uIndex);

   //! returns element the attribute belongs to
   XmlElementPtr OwnerElement(){ return m_spOwnerElement; }

   // methods

   //! appends given attribute to collection
   void Append(XmlAttributePtr spAttribute)
   {
      spAttribute->OwnerElement(OwnerElement());
      m_mapAllAttributes.insert(std::make_pair(spAttribute->Name(), spAttribute));
   }

   //! copies attributes to given vector
   void CopyTo(std::vector<XmlAttributePtr>& vecSpAttributes);

   //! returns number of attributes in collection
   size_t Count() const { return m_mapAllAttributes.size(); }

   //! removes attribute by attribute ptr
   void Remove(XmlAttributePtr spAttribute){ Remove(spAttribute->Name()); }

   //! removes attribute by name
   void Remove(const CString& cszAttrName);

   //! removes all attributes
   void RemoveAll(){ m_mapAllAttributes.clear(); }

private:
   friend class XmlElement;

   //! map type of attribute names to attribute nodes
   typedef std::map<CString, XmlAttributePtr> T_MapAllAttributes;

   //! returns attribute map
   const T_MapAllAttributes GetAttributeMap() const { return m_mapAllAttributes; }

   //! ctor
   XmlAttributeCollection(XmlElementPtr spOwnerElement)
      :m_spOwnerElement(spOwnerElement)
   {
   }

private:
   //! map with all attribute nodes
   T_MapAllAttributes m_mapAllAttributes;

   //! owner element
   XmlElementPtr m_spOwnerElement;
};

//! xml element; can contain attributes and subnodes
class XmlElement: public XmlNode, public std::enable_shared_from_this<XmlElement>
{
public:
   // attributes

   //! returns attribute collection
   XmlAttributeCollectionPtr Attributes();

   //! indicates if the element is an empty one, i.e. it contains no further nodes
   bool IsEmpty() const { return !HasChildNodes(); }

   /// returns if the element has attributes
   bool HasAttributes() const { return m_spAllAttributes.get() == NULL ? false : m_spAllAttributes->Count() != 0; }

   // methods

   /// returns attribute value by attribute name
   CString GetAttribute(const CString& cszAttrName) const;

   /// returns attribute node by attribute name
   XmlAttributePtr GetAttributeNode(const CString& cszAttrName);

   /// returns all element nodes by given tag name
   XmlNodeListPtr GetElementsByTagName(const CString& cszTagName);

   /// removes all attributes and child nodes
   virtual void RemoveAll()
   {
      RemoveAllAttributes();
      XmlNode::RemoveAll();
   }

   /// removes all attributes
   void RemoveAllAttributes()
   {
      if (m_spAllAttributes.get() != NULL)
         m_spAllAttributes->RemoveAll();
   }

   //! removes attribute by name
   void RemoveAttribute(const CString& cszAttribute);

   //! sets attribute value; attribute is created if it doesn't exist yet
   void SetAttribute(const CString& cszAttrName, const CString& cszValue);

   //! inserts or sets attribute node
//   void SetAttributeNode(XmlAttributePtr spAttribute);

   //! writes element
   virtual void WriteTo(XmlWriterPtr spXmlWriter) const;

protected:
   friend class XmlDocument;

   //! ctor; creates new named element
   XmlElement(const CString& cszName, XmlDocumentPtr spDocument)
      :XmlNode(spDocument, nodeTypeElement)
   {
      Name(cszName);
   }

private:
   //! attribute collection
   XmlAttributeCollectionPtr m_spAllAttributes;
};


//! node list
class XmlNodeList
{
public:
   // attributes

   //! returns node count
   size_t Count() const{ return m_vecAllNodes.size(); }

   //! returns node from list
   XmlNodePtr ItemOf(size_t iIndex)
   {
      ATLASSERT(iIndex < m_vecAllNodes.size());
      return m_vecAllNodes[iIndex];
   }

   // methods

   //! appends node to node list
   void Append(XmlNodePtr spNode){ m_vecAllNodes.push_back(spNode); }

   //! prepends node to node list
   void Prepend(XmlNodePtr spNode){ m_vecAllNodes.insert(m_vecAllNodes.begin(), spNode); }

   //! removes all nodes
   void RemoveAll(){ m_vecAllNodes.clear(); }

private:
   //! node list
   std::vector<XmlNodePtr> m_vecAllNodes;
};

/// xml node that contains plain text
class XmlText: public XmlNode
{
public:
   //XmlNodePtr SplitText(int iOffset);

   virtual void WriteTo(XmlWriterPtr spXmlWriter) const;

protected:
   friend class XmlDocument;

   XmlText(XmlDocumentPtr spDocument)
      :XmlNode(spDocument, nodeTypeText)
   {
   }
};

/// xml comment node
class XmlComment: public XmlNode
{
public:
   virtual void WriteTo(XmlWriterPtr spXmlWriter) const;


protected:
   friend class XmlDocument;

   XmlComment(XmlDocumentPtr spDocument)
      :XmlNode(spDocument, nodeTypeComment)
   {
   }
};

/// \brief CDATA section node
/// CDATA section contents can be get/set via Value()
class XmlCDataSection: public XmlNode
{
public:
   /// writes CDATA section
   virtual void WriteTo(XmlWriterPtr spXmlWriter) const;

protected:
   friend class XmlDocument;

   /// ctor
   XmlCDataSection(XmlDocumentPtr spDocument)
      :XmlNode(spDocument, nodeTypeCDataSection)
   {
   }
};


/// xml declaration (<?xml ?> at start of xml files)
class XmlDeclaration: public XmlNode, public std::enable_shared_from_this<XmlDeclaration>
{
public:
   /// returns version
   const CString& Version() const throw() { return m_cszVersion; }

   /// returns encoding field of xml declaration
   const CString Encoding() const throw() { return m_cszEncoding; }

   /// returns if this is a standalone document
   bool Standalone() const throw() { return m_bStandalone; }

   /// writes declaration to xml writer
   virtual void WriteTo(XmlWriterPtr spXmlWriter) const;

private:
   friend class XmlDocument;

   /// ctor
   XmlDeclaration(XmlDocumentPtr spDocument, const CString& cszVersion, bool bStandalone)
      :XmlNode(spDocument, XmlNode::nodeTypeXmlDeclaration),
       m_cszVersion(cszVersion),
       m_bStandalone(bStandalone)
   {
   }

private:
   /// xml version; always "1.0"
   CString m_cszVersion;

   /// xml encoding
   CString m_cszEncoding;

   /// indicates if xml is a standalone file
   bool m_bStandalone;
};

/// xml document
class XmlDocument: public XmlNode, public std::enable_shared_from_this<XmlDocument>
{
public:
   // attributes
   XmlElementPtr DocumentElement() throw() { return m_spDocumentElement; }
   const XmlElementPtr DocumentElement() const throw() { return m_spDocumentElement; }

   void DocumentElement(XmlElementPtr spElement){ m_spDocumentElement = spElement; }

   //XmlSchemaCollectionPtr Schemas();

   // methods

   XmlAttributePtr CreateAttribute(const CString& cszAttrName);

   XmlElementPtr CreateElement(const CString& cszElementName);

   XmlTextPtr CreateText(const CString& cszText);

   XmlCommentPtr CreateComment(const CString& cszComment);

   XmlCDataSectionPtr CreateCDATASection(const CString& cszCDATA);

   XmlDeclarationPtr CreateXmlDeclaration(const CString& cszVersion = _T("1.0"), bool bStandalone = true);

   //ImportNode

   //! loads the xml document from the given filename
   void Load(const CString& cszFilename);

   //! loads the xml document from given reader
   void Load(XmlReaderPtr spXmlReader);

   //! loads xml data from given string
   void LoadXml(const CString& cszXmlText);

   //! saves xml document to string and returns it in cszXmlText
   void Save(CString& cszXmlText);

   void Save(XmlWriterPtr cszXmlText);

   virtual void WriteTo(XmlWriterPtr spXmlWriter) const;

   //
   //bool Validate();

   static XmlDocumentPtr Create(){ return XmlDocumentPtr(new XmlDocument()); }

private:
   XmlDocument()
      :XmlNode(XmlDocumentPtr(), nodeTypeDocument)
   {
   }

private:
   XmlElementPtr m_spDocumentElement;
};

/// naviagates xml nodes given XPath expression
class XPathNavigator
{
public:
   XmlNodePtr Evaluate(const CString& cszXPathExpression);

   XmlNodeListPtr Select(const CString& cszXPathExpression);

private:
};


/// \brief xml name table
/// manages all node names using ids
class XmlNameTable: private std::map<unsigned int, CString>
{
public:
   XmlNameTable(){}

};

typedef std::shared_ptr<class XmlNameTable> XmlNameTablePtr;


/// manages namespace / prefix pairs
class XmlNamespaceManager
{
public:
   XmlNamespaceManager(XmlNameTablePtr spNameTable)
      :m_spNameTable(spNameTable)
   {
   }

   // properties

   XmlNameTablePtr NameTable() const throw() { return m_spNameTable; }

   // methods

   void AddNamespace(const CString& cszPrefix, const CString& cszURI);

   bool HasNamespace(const CString& cszPrefix) const throw();

   CString LookupNamespace(const CString& cszPrefix) const throw();

   CString LookupPrefix(const CString& cszPrefix) const throw();

   void PushScope();

   bool PopScope();

   void RemoveNamespace(const CString& cszPrefix, const CString& cszURI);

private:
   XmlNameTablePtr m_spNameTable;
};




// all inline methods

inline XmlNodePtr XmlNode::FirstChild()
{
   return (!m_spAllChildNodes || m_spAllChildNodes->Count() == 0) ? XmlNodePtr() : m_spAllChildNodes->ItemOf(0);
}

inline bool XmlNode::HasChildNodes() const
{
   return m_spAllChildNodes.get() == NULL ? false : m_spAllChildNodes->Count() != 0;
}

inline void XmlNode::RemoveAll()
{
   if (m_spAllChildNodes)
      m_spAllChildNodes->RemoveAll();
}

// template specialisations for XmlNode::CastNode()

template<> inline std::shared_ptr<class XmlDeclaration> XmlNode::CastNode()
{
   return NodeType() == nodeTypeXmlDeclaration ?
      reinterpret_cast<XmlDeclaration*>(this)->shared_from_this() :
      std::shared_ptr<class XmlDeclaration>();
}

template<> inline std::shared_ptr<class XmlElement> XmlNode::CastNode()
{
   return NodeType() == nodeTypeElement ?
      reinterpret_cast<XmlElement*>(this)->shared_from_this() :
      std::shared_ptr<class XmlElement>();
}



} // namespace XML
