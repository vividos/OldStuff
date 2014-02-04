//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file XmlWriter.cpp xml writer class
//
#include "stdafx.h"
#include <ulib/xml/XmlWriter.hpp>
#include <ulib/xml/Xml.hpp>
#include <ulib/stream/ITextStream.hpp>

using XML::XmlWriter;

XmlWriter::XmlWriter(::Stream::ITextStream& textStream)
:m_textStream(textStream)
{
}

CString EncodeXMLChars(const CString& cszText)
{
   CString cszNewText(cszText);
   cszNewText.Replace(_T("<"), _T("&lt;"));
   cszNewText.Replace(_T(">"), _T("&gt;"));
   return cszNewText;
}

void XmlWriter::WriteAttributeString(const CString& cszName, const CString& cszAttr)
{
   ATLASSERT(!m_vecElementStack.empty());

   Write(_T(" ") + cszName + _T("=\"") + EncodeXMLChars(cszAttr) + _T("\""));
}

/// \todo do cdata encoding
void XmlWriter::WriteCData(const CString& cszText)
{
   ATLASSERT(!m_vecElementStack.empty());

   // TODO do cdata encoding when needed
   Write(EncodeXMLChars(cszText));

   m_vecElementStack.back().HasContent(true);
}

/// \todo do comment encoding
void XmlWriter::WriteComment(const CString& cszText)
{
   ATLASSERT(!m_vecElementStack.empty());

   // TODO do comment encoding when needed
   Write(_T("<--"));
   Write(cszText);
   Write(_T("-->"));

   m_vecElementStack.back().HasContent(true);
}

void XmlWriter::WriteElementString(const CString& cszName, const CString& cszValue)
{
   WriteStartElement(cszName);
   WriteStartElementEnd();
   WriteCData(cszValue);
   WriteEndElement();
}

void XmlWriter::WriteEndDocument()
{
   // walk back the stack and close all elements
   while (!m_vecElementStack.empty())
      WriteEndElement();
}

void XmlWriter::WriteEndElement()
{
   ATLASSERT(!m_vecElementStack.empty());

   if (m_vecElementStack.back().HasContent())
      WriteFullEndElement();
   else
   {
      Write(_T("/>"));
      m_vecElementStack.pop_back();
   }
}

void XmlWriter::WriteFullEndElement()
{
   ATLASSERT(!m_vecElementStack.empty());

   Write(_T("</") + m_vecElementStack.back().ElementName() + _T(">"));

   m_vecElementStack.pop_back();
}
/*
void XmlWriter::WriteStartDocument(XML::XmlConstDocumentPtr spDoc)
{
   // search for XmlDeclaration and write it
   const XML::XmlNodeListPtr spNodes = spDoc->ChildNodes();
   if (spNodes == NULL)
      return; // nothing to do

   unsigned int uStage = 0;

   for (size_t i=0, iMax = spNodes->Count(); i<iMax; i++)
   {
      // scan nodes until an element node occurs (the root node)
      XmlNodePtr spNode = spNodes->ItemOf(i);
      if (spNode->NodeType() == XML::XmlNode::nodeTypeElement)
         break;

      switch (uStage)
      {
      case 0: // in stage 0, we search for XmlDeclaration
         if (spNode->NodeType() == XML::XmlNode::nodeTypeXmlDeclaration)
         {
            // found one! next stage: 1
            uStage = 1;

            // cast to xml decl
            XmlDeclarationPtr spDecl = spNode->CastNode<XmlDeclaration>();

            WriteXmlDeclaration(spDecl->Version(), spDecl->Standalone());
         }
         break;

      case 1: // in stage 1, we search for XmlProcessingInstruction
         if (spNode->NodeType() == XML::XmlNode::nodeTypeProcessingInstruction)
         {
            // note that there can be more than one PI, so keep scanning until we encounter the root element

            // TODO cast to PI
            //XmlProcessingInstructionPtr spPI = spNode->CastNode<XmlProcessingInstruction>();
            // TODO write out
         }
      }
   }
}
*/
void XmlWriter::WriteStartElement(const CString& cszElementName)
{
   ATLASSERT(!cszElementName.IsEmpty());

   Write(_T("<") + cszElementName);

   // add element info to stack
   XmlWriterElementInfo elementInfo(cszElementName);
   m_vecElementStack.push_back(elementInfo);
}

void XmlWriter::WriteStartElementEnd()
{
   ATLASSERT(!m_vecElementStack.empty());

   Write(_T(">"));

   // and since we closed the start tag, the node must have content
   m_vecElementStack.back().HasContent(true);
}

void XmlWriter::WriteXmlDeclaration(const CString& cszVersion, bool bStandalone)
{
   Write(_T("<?xml"));

   XmlWriterElementInfo elementInfo(_T("?xml"));
   m_vecElementStack.push_back(elementInfo);

   WriteAttributeString(_T("version"), cszVersion);

   CString cszEncoding;
   switch (m_textStream.TextEncoding())
   {
   case Stream::ITextStream::textEncodingUTF8:
      cszEncoding = _T("UTF-8");
      break;

   case Stream::ITextStream::textEncodingUCS2:
      cszEncoding = _T("UTF-16"); // TODO
      break;

   case Stream::ITextStream::textEncodingAnsi:
      cszEncoding = _T("ISO-8859-1"); // TODO
      break;

   default:
      ATLASSERT(false);
      break;
   }

   WriteAttributeString(_T("encoding"), cszEncoding);

   WriteAttributeString(_T("standalone"), bStandalone ? _T("yes") : _T("no"));
   Write(_T("?>"));

   m_vecElementStack.pop_back();
}
/*
#include <ulib/xml/Xml.hpp>

void XmlWriter::WriteNode(const XML::XmlNodePtr spNode)
{
   switch(spNode->NodeType())
   {
   case XML::XmlNode::nodeTypeElement:     //!< element case node
      // mark the current element as 'has content', if there is any
      if (!m_vecElementStack.empty())
      {
         if (!m_vecElementStack.back().HasContent())
         {
            // doesn't have content yet
            WriteStartElementEnd();
            m_vecElementStack.back().HasContent(true);
         }
      }

      {
         WriteStartElement(spNode->Name());
         XmlElementPtr spElement = spNode->CastNode<XmlElement>();

         // write all attributes
         if (spElement->HasAttributes())
         {
            XmlAttributeCollectionPtr spAttributes = spElement->Attributes();
            for (size_t i=0,iMax = spAttributes->Count(); i<iMax; i++)
            {
               XmlAttributePtr spAttr = spAttributes->ItemOf(i);
               WriteAttributeString(spAttr->Name(), spAttr->Value());
            }
         }
         // note: we don't call WriteStartElementEnd() here, since it could be a node without childs
      }
      break;

   case XML::XmlNode::nodeTypeText:        //!< text case node
      // mark the current element as 'has content'
      // note that text can only appear inside of root node
      ATLASSERT(!m_vecElementStack.empty());

      if (!m_vecElementStack.back().HasContent())
      {
         // doesn't have content yet
         WriteStartElementEnd();
         m_vecElementStack.back().HasContent(true);
      }

      WriteEscaped(spNode->Value());
      break;

   case XML::XmlNode::nodeTypeCDataSection: // CDATA section case node "<!CDATA[[{1}]]>
      // mark the current element as 'has content'
      // note that text can only appear inside of root node
      ATLASSERT(!m_vecElementStack.empty());

      if (!m_vecElementStack.back().HasContent())
      {
         // doesn't have content yet
         WriteStartElementEnd();
         m_vecElementStack.back().HasContent(true);
      }

      Write(_T("<!CDATA[["));
      WriteEscaped(spNode->Value());
      Write(_T("]]>"));
      break;

   case XML::XmlNode::nodeTypeProcessingInstruction: //!< PI case node "<?{0} {1}?>"
      ATLASSERT(false); // TODO implement
      break;

   case XML::XmlNode::nodeTypeComment:     //!< comment "<!--{1}-->
      // mark the current element as 'has content', if there is any
      if (!m_vecElementStack.empty())
      {
         if (!m_vecElementStack.back().HasContent())
         {
            // doesn't have content yet
            WriteStartElementEnd();
            m_vecElementStack.back().HasContent(true);
         }
      }

      Write(_T("<!--"));
      Write(spNode->Value());
      Write(_T("-->"));
      break;

   case XML::XmlNode::nodeTypeXmlDeclaration: //!< <?xml version="1.0"?>
      {
         ATLASSERT(m_vecElementStack.empty()); // stack must be empty

         // cast to xml decl
         XmlDeclarationPtr spDecl = spNode->CastNode<XmlDeclaration>();
         WriteXmlDeclaration(spDecl->Version(), spDecl->Standalone());
      }
      break;

   case XML::XmlNode::nodeTypeDocument:    //!< document case node
      ATLASSERT(false);
      break;

   case XML::XmlNode::nodeTypeEndElement:  //!< end element </{0}>
      // check if we have the same element name as the currently open element
      ATLASSERT(!m_vecElementStack.empty());
      ATLASSERT(m_vecElementStack.back().ElementName() == spNode->Name());
      WriteEndElement();
      break;

   case XML::XmlNode::nodeTypeInvalid:      //!< invalid
   default:
      //throw XmlException;
      ATLASSERT(false);
      break;
   }
}
*/

void XmlWriter::WriteEscaped(const CString& cszText)
{
   // TODO do escaping
   Write(cszText);
}

void XmlWriter::Write(const CString& cszText)
{
   m_textStream.Write(cszText);
}
