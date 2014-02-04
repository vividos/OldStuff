//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file XmlWriter.hpp xml writer class
//
#pragma once

// includes
#include <memory>
#include <vector>

// forward references
namespace Stream
{
   class ITextStream;
}

namespace XML
{

/// smart pointer to xml writer
typedef std::shared_ptr<class XmlWriter> XmlWriterPtr;
/// smart pointer to xml node
typedef std::shared_ptr<class XmlNode> XmlNodePtr;
/// smart pointer to xml document
typedef std::shared_ptr<class XmlDocument> XmlDocumentPtr;
/// smart pointer to const xml document
typedef std::shared_ptr<const class XmlDocument> XmlConstDocumentPtr;


/// settings for the xml writer
class XmlWriterSettings
{
public:
   /// ctor
   XmlWriterSettings() throw()
      :m_bIndent(false)
   {
   }

   /// returns if idendation should be used
   bool Indent() const throw() { return m_bIndent; }

   /// sets if idendation should be used
   void Indent(bool bIndent){ m_bIndent = bIndent; }

private:
   /// indicates if idendation should be used; default: false
   bool m_bIndent;
};

/// xml writer
/// The writer class can be used to directly produce xml output without needing
/// to have an actual DOM document, e.g. it can be used for exporting to xml
/// from another data structure.
/// \todo not completely implemented yet
class XmlWriter
{
public:
   /// ctor; takes stream to write to
   XmlWriter(::Stream::ITextStream& textStream) throw();

   /// ctor; takes stream to write to and settings
   XmlWriter(::Stream::ITextStream& textStream, XmlWriterSettings settings) throw();

   // attributes

   /// returns xml writer settings
   XmlWriterSettings& Settings() throw() { return m_settings; }

   // methods

   /// writes attribute and value
   void WriteAttributeString(const CString& cszName, const CString& cszAttr);

   /// writes cdata text
   void WriteCData(const CString& cszText);

   /// writes comment text
   void WriteComment(const CString& cszText);

   /// writes element with text as subelement
   void WriteElementString(const CString& cszName, const CString& cszValue);

   /// closes document by closing all remaining open elements
   void WriteEndDocument();

   /// writes end element
   void WriteEndElement();

   /// writes full end element, and not just an empty "/>" one when possible
   void WriteFullEndElement();

   /// writes start of document (xml decl, parsing instructions, etc.)
   void WriteStartDocument(XmlConstDocumentPtr spDoc);

   /// writes start of element with given name
   void WriteStartElement(const CString& cszElementName);

   /// writes end of start element tag; only use in case the element contains sub-nodes
   void WriteStartElementEnd();

   /// writes xml declaration
   void WriteXmlDeclaration(const CString& cszVersion, bool bStandalone);

   /// writes raw node
   void WriteNode(const XmlNodePtr spNode);

private:
   /// writes out escaped text
   void WriteEscaped(const CString& cszText);

   /// writes out text
   void Write(const CString& cszText);

private:
   /// text stream to use to write out xml
   Stream::ITextStream& m_textStream;

   /// writer settings
   XmlWriterSettings m_settings;

   /// element info for xml writer
   class XmlWriterElementInfo
   {
   public:
      /// ctor
      XmlWriterElementInfo(const CString& cszElementName) throw()
         :m_cszElementName(cszElementName),
         m_bHasContent(false)
      {
      }

      /// returns element name
      CString ElementName() const throw() { return m_cszElementName; }

      /// indicates if element has content
      bool HasContent() const throw() { return m_bHasContent; }

      /// sets flag that element has content
      void HasContent(bool bHasContent) throw() { m_bHasContent = bHasContent; }

   private:
      /// element name
      CString m_cszElementName;

      /// indicates if element has content
      bool m_bHasContent;
   };

   /// list of open elements
   std::vector<XmlWriterElementInfo> m_vecElementStack;
};

} // namespace XML
