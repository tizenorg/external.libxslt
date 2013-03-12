/*
 * Summary: internal data structures, constants and functions
 * Description: Internal data structures, constants and functions used
 *              by the XSLT engine. 
 *              They are not part of the API or ABI, i.e. they can change
 *              without prior notice, use carefully.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 */

#ifndef __XML_XSLT_INTERNALS_H__
#define __XML_XSLT_INTERNALS_H__

#include <libxml/tree.h>
#include <libxml/hash.h>
#include <libxml/xpath.h>
#include <libxml/xmlerror.h>
#include <libxml/dict.h>
#include <libxslt/xslt.h>
#include "xsltexports.h"
#include "numbersInternals.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * XSLT_REFACTORED:
 *
 * Internal define to enable the refactored parts of Libxslt
 * mostly related to pre-computation.
 */
/* #define XSLT_REFACTORED */

/**
 * XSLT_REFACTORED_PARSING:
 *
 * Internal define to enable the refactored parts of Libxslt
 * related to parsing.
 */
/* #define XSLT_REFACTORED_PARSING */

/**
 * XSLT_MAX_SORT:
 *
 * Max number of specified xsl:sort on an element.
 */
#define XSLT_MAX_SORT 15

/**
 * XSLT_PAT_NO_PRIORITY:
 *
 * Specific value for pattern without priority expressed.
 */
#define XSLT_PAT_NO_PRIORITY -12345789

/**
 * xsltRuntimeExtra:
 *
 * Extra information added to the transformation context.
 */
typedef struct _xsltRuntimeExtra xsltRuntimeExtra;
typedef xsltRuntimeExtra *xsltRuntimeExtraPtr;
struct _xsltRuntimeExtra {
    void       *info;		/* pointer to the extra data */
    xmlFreeFunc deallocate;	/* pointer to the deallocation routine */
    union {			/* dual-purpose field */
        void   *ptr;		/* data not needing deallocation */
	int    ival;		/* integer value storage */
    } val;
};

/**
 * XSLT_RUNTIME_EXTRA_LST:
 * @ctxt: the transformation context
 * @nr: the index
 *
 * Macro used to access extra information stored in the context
 */
#define XSLT_RUNTIME_EXTRA_LST(ctxt, nr) (ctxt)->extras[(nr)].info
/**
 * XSLT_RUNTIME_EXTRA_FREE:
 * @ctxt: the transformation context
 * @nr: the index
 *
 * Macro used to free extra information stored in the context
 */
#define XSLT_RUNTIME_EXTRA_FREE(ctxt, nr) (ctxt)->extras[(nr)].deallocate
/**
 * XSLT_RUNTIME_EXTRA:
 * @ctxt: the transformation context
 * @nr: the index
 *
 * Macro used to define extra information stored in the context
 */
#define	XSLT_RUNTIME_EXTRA(ctxt, nr, typ) (ctxt)->extras[(nr)].val.typ

/**
 * xsltTemplate:
 *
 * The in-memory structure corresponding to an XSLT Template.
 */
typedef struct _xsltTemplate xsltTemplate;
typedef xsltTemplate *xsltTemplatePtr;
struct _xsltTemplate {
    struct _xsltTemplate *next;/* chained list sorted by priority */
    struct _xsltStylesheet *style;/* the containing stylesheet */
    xmlChar *match;	/* the matching string */
    float priority;	/* as given from the stylesheet, not computed */
    xmlChar *name;	/* the local part of the name QName */
    xmlChar *nameURI;	/* the URI part of the name QName */
    const xmlChar *mode;/* the local part of the mode QName */
    const xmlChar *modeURI;/* the URI part of the mode QName */
    xmlNodePtr content;	/* the template replacement value */
    xmlNodePtr elem;	/* the source element */

    int inheritedNsNr;  /* number of inherited namespaces */
    xmlNsPtr *inheritedNs;/* inherited non-excluded namespaces */

    /* Profiling informations */
    int nbCalls;        /* the number of time the template was called */
    unsigned long time; /* the time spent in this template */
};

/**
 * xsltDecimalFormat:
 *
 * Data structure of decimal-format.
 */
typedef struct _xsltDecimalFormat xsltDecimalFormat;
typedef xsltDecimalFormat *xsltDecimalFormatPtr;
struct _xsltDecimalFormat {
    struct _xsltDecimalFormat *next; /* chained list */
    xmlChar *name;
    /* Used for interpretation of pattern */
    xmlChar *digit;
    xmlChar *patternSeparator;
    /* May appear in result */
    xmlChar *minusSign;
    xmlChar *infinity;
    xmlChar *noNumber; /* Not-a-number */
    /* Used for interpretation of pattern and may appear in result */
    xmlChar *decimalPoint;
    xmlChar *grouping;
    xmlChar *percent;
    xmlChar *permille;
    xmlChar *zeroDigit;
};

/**
 * xsltDocument:
 *
 * Data structure associated to a parsed document.
 */

typedef struct _xsltDocument xsltDocument;
typedef xsltDocument *xsltDocumentPtr;
struct _xsltDocument {
    struct _xsltDocument *next;	/* documents are kept in a chained list */
    int main;			/* is this the main document */
    xmlDocPtr doc;		/* the parsed document */
    void *keys;			/* key tables storage */
    struct _xsltDocument *includes; /* subsidiary includes */
    int preproc;		/* pre-processing already done */
};

/*
 * The in-memory structure corresponding to an XSLT Stylesheet.
 * NOTE: most of the content is simply linked from the doc tree
 *       structure, no specific allocation is made.
 */
typedef struct _xsltStylesheet xsltStylesheet;
typedef xsltStylesheet *xsltStylesheetPtr;

typedef struct _xsltTransformContext xsltTransformContext;
typedef xsltTransformContext *xsltTransformContextPtr;

/**
 * xsltElemPreComp:
 *
 * The in-memory structure corresponding to element precomputed data,
 * designed to be extended by extension implementors.
 */
typedef struct _xsltElemPreComp xsltElemPreComp;
typedef xsltElemPreComp *xsltElemPreCompPtr;

/**
 * xsltTransformFunction:
 * @ctxt: the XSLT transformation context
 * @node: the input node
 * @inst: the stylesheet node
 * @comp: the compiled information from the stylesheet
 *
 * Signature of the function associated to elements part of the
 * stylesheet language like xsl:if or xsl:apply-templates.
 */
typedef void (*xsltTransformFunction) (xsltTransformContextPtr ctxt,
	                               xmlNodePtr node,
				       xmlNodePtr inst,
			               xsltElemPreCompPtr comp);

/**
 * xsltSortFunc:
 * @ctxt:    a transformation context
 * @sorts:   the node-set to sort
 * @nbsorts: the number of sorts
 *
 * Signature of the function to use during sorting
 */
typedef void (*xsltSortFunc) (xsltTransformContextPtr ctxt, xmlNodePtr *sorts,
			      int nbsorts);

typedef enum {
    XSLT_FUNC_COPY=1,
    XSLT_FUNC_SORT,
    XSLT_FUNC_TEXT,
    XSLT_FUNC_ELEMENT,
    XSLT_FUNC_ATTRIBUTE,
    XSLT_FUNC_COMMENT,
    XSLT_FUNC_PI,
    XSLT_FUNC_COPYOF,
    XSLT_FUNC_VALUEOF,
    XSLT_FUNC_NUMBER,
    XSLT_FUNC_APPLYIMPORTS,
    XSLT_FUNC_CALLTEMPLATE,
    XSLT_FUNC_APPLYTEMPLATES,
    XSLT_FUNC_CHOOSE,
    XSLT_FUNC_IF,
    XSLT_FUNC_FOREACH,
    XSLT_FUNC_DOCUMENT,
    XSLT_FUNC_WITHPARAM,
    XSLT_FUNC_PARAM,
    XSLT_FUNC_VARIABLE,
    XSLT_FUNC_WHEN,
    XSLT_FUNC_EXTENSION,
    XSLT_FUNC_OTHERWISE,
} xsltStyleType;

/**
 * xsltElemPreCompDeallocator:
 * @comp:  the #xsltElemPreComp to free up
 *
 * Deallocates an #xsltElemPreComp structure.
 */
typedef void (*xsltElemPreCompDeallocator) (xsltElemPreCompPtr comp);

/**
 * xsltElemPreComp:
 *
 * The basic structure for compiled items of the AST of the XSLT processor.
 * This structure is also intended to be extended by extension implementors.
 * TODO: This is somehow not nice, since it has a "free" field, which
 *   derived stylesheet-structs do not have.
 */
struct _xsltElemPreComp {
    xsltElemPreCompPtr next;		/* next item in the global chained
					   list hold by xsltStylesheet. */
    xsltStyleType type;			/* type of the element */
    xsltTransformFunction func; 	/* handling function */
    xmlNodePtr inst;			/* the node in the stylesheet's tree
					   corresponding to this item */

    /* end of common part */
    xsltElemPreCompDeallocator free;	/* the deallocator */
};

/**
 * xsltStylePreComp:
 *
 * The abstract basic structure for items of the
 * AST of the XSLT processor.
 * The AST includes:
 * 1) compiled forms of XSLT instructions (xsl:if, xsl:attribute, etc.)
 * 2) compiled forms of literal result elements
 */
typedef struct _xsltStylePreComp xsltStylePreComp;
typedef xsltStylePreComp *xsltStylePreCompPtr;

/*************************
 * Refactored structures *
 *************************/
#ifdef XSLT_REFACTORED

typedef struct _xsltNsList xsltNsList;

typedef xsltNsList *xsltNsListPtr;
struct _xsltNsList {
    xmlNsPtr *list;
    int number;
};

#if 0
/*
 * TODO: xsltBasicItem is not used yet; maybe never will be used, since
 * xsltElemPreCompPtr is acting as the base type for the compiled
 * items of a stylesheet. It seems not practical to try to change
 * this type to xsltBasicItemPtr, since xsltElemPreCompPtr is
 * used already used too massively (e.g. xsltStylesheet->preComps) and
 * for extension functions.
 */
/**
 * xsltBasicItem:
 *
 * The basic structure for all items of the AST of the XSLT processor.
 */
typedef struct _xsltBasicItem xsltBasicItem;

typedef xsltBasicItem *xsltBasicItemPtr;
struct _xsltBasicItem {
    xsltBasicASTItemPtr next;
    xsltStyleType type;
};
#endif

/**
 * XSLT_ITEM_COMPATIBILITY_FIELDS:
 * 
 * Fields for API compatibility to the structure
 * _xsltElemPreComp which is used for extension functions.
 * TODO: Evaluate if we really need such a compatibility.
 */
#define XSLT_ITEM_COMPATIBILITY_FIELDS \
    xsltElemPreCompPtr next;\
    xsltStyleType type;\
    xsltTransformFunction func;\
    xmlNodePtr inst;

/**
 * XSLT_ITEM_NAVIGATION_FIELDS:
 *
 * Currently empty.
 * TODO: It is intended to hold navigational fields in the future.
 */
#define XSLT_ITEM_NAVIGATION_FIELDS
/*
    xsltStylePreCompPtr parent;\
    xsltStylePreCompPtr children;\
    xsltStylePreCompPtr nextItem; 
*/

/**
 * XSLT_ITEM_NSINSCOPE_FIELDS:
 *
 * The in-scope namespaces.
 */
#define XSLT_ITEM_NSINSCOPE_FIELDS xsltNsListPtr inScopeNS;

/**
 * XSLT_ITEM_COMMON_FIELDS:
 *
 * Common fields used for all items.
 */
#define XSLT_ITEM_COMMON_FIELDS \
    XSLT_ITEM_COMPATIBILITY_FIELDS \
    XSLT_ITEM_NAVIGATION_FIELDS \
    XSLT_ITEM_NSINSCOPE_FIELDS

/**
 * _xsltStylePreComp: 
 *
 * The abstract basic structure for items of the XSLT processor.
 * This includes:
 * 1) compiled forms of XSLT instructions (e.g. xsl:if, xsl:attribute, etc.)
 * 2) compiled forms of literal result elements
 * 3) various properties for XSLT instructions (e.g. xsl:when,
 *    xsl:with-param)
 *
 * REVISIT TODO: Keep this structure equal to the fields
 *   defined by XSLT_ITEM_COMMON_FIELDS
 */
struct _xsltStylePreComp {
    xsltElemPreCompPtr next;    /* next item in the global chained
				   list hold by xsltStylesheet */
    xsltStyleType type;         /* type of the item */ 
    xsltTransformFunction func; /* handling function */
    xmlNodePtr inst;		/* the node in the stylesheet's tree
				   corresponding to this item. */
    /* Currenlty to navigational fields. */
    xsltNsListPtr inScopeNS;
};

/**
 * xsltStyleBasicEmptyItem:
 * 
 * Abstract structure only used as a short-cut for
 * XSLT items with no extra fields.
 * NOTE that it is intended that this structure looks the same as
 *  _xsltStylePreComp.
 */
typedef struct _xsltStyleBasicEmptyItem xsltStyleBasicEmptyItem;
typedef xsltStyleBasicEmptyItem *xsltStyleBasicEmptyItemPtr;

struct _xsltStyleBasicEmptyItem {
    XSLT_ITEM_COMMON_FIELDS
};

/**
 * xsltStyleBasicExpressionItem:
 * 
 * Abstract structure only used as a short-cut for
 * XSLT items with just an expression.
 */
typedef struct _xsltStyleBasicExpressionItem xsltStyleBasicExpressionItem;
typedef xsltStyleBasicExpressionItem *xsltStyleBasicExpressionItemPtr;

struct _xsltStyleBasicExpressionItem {
    XSLT_ITEM_COMMON_FIELDS

    const xmlChar *select; /* TODO: Change this to "expression". */
    xmlXPathCompExprPtr comp; /* TODO: Change this to compExpr. */
};

/************************************************************************
 *									*
 * XSLT-instructions/declarations                                       *
 *									*
 ************************************************************************/

/**
 * xsltStyleItemElement:
 * 
 * <!-- Category: instruction -->
 * <xsl:element
 *  name = { qname }
 *  namespace = { uri-reference }
 *  use-attribute-sets = qnames>
 *  <!-- Content: template -->
 * </xsl:element>
 */
typedef struct _xsltStyleItemElement xsltStyleItemElement;
typedef xsltStyleItemElement *xsltStyleItemElementPtr;

struct _xsltStyleItemElement {
    XSLT_ITEM_COMMON_FIELDS 

    const xmlChar *use;		/* copy, element */
    int      has_use;		/* copy, element */
    const xmlChar *name;	/* element, attribute, pi */
    int      has_name;		/* element, attribute, pi */
    const xmlChar *ns;		/* element */
    int      has_ns;		/* element */

};

/**
 * xsltStyleItemAttribute:
 *
 * <!-- Category: instruction -->
 * <xsl:attribute
 *  name = { qname }
 *  namespace = { uri-reference }>
 *  <!-- Content: template -->
 * </xsl:attribute>
 */
typedef struct _xsltStyleItemAttribute xsltStyleItemAttribute;
typedef xsltStyleItemAttribute *xsltStyleItemAttributePtr;

struct _xsltStyleItemAttribute {
    XSLT_ITEM_COMMON_FIELDS
    const xmlChar *name;	/* element, attribute, pi */
    int      has_name;		/* element, attribute, pi */
    const xmlChar *ns;		/* element  attribute */
    int      has_ns;		/* element  attribute */
};

/**
 * xsltStyleItemText:
 *
 * <!-- Category: instruction -->
 * <xsl:text
 *  disable-output-escaping = "yes" | "no">
 *  <!-- Content: #PCDATA -->
 * </xsl:text>
 */
typedef struct _xsltStyleItemText xsltStyleItemText;
typedef xsltStyleItemText *xsltStyleItemTextPtr;

struct _xsltStyleItemText {
    XSLT_ITEM_COMMON_FIELDS
    int      noescape;		/* text */
};

/**
 * xsltStyleItemComment:
 *
 * <!-- Category: instruction -->
 *  <xsl:comment>
 *  <!-- Content: template -->
 * </xsl:comment>
 */
typedef xsltStyleBasicEmptyItem xsltStyleItemComment;
typedef xsltStyleItemComment *xsltStyleItemCommentPtr;

/**
 * xsltStyleItemPI:
 *
 * <!-- Category: instruction -->
 *  <xsl:processing-instruction
 *  name = { ncname }>
 *  <!-- Content: template -->
 * </xsl:processing-instruction>
 */
typedef struct _xsltStyleItemPI xsltStyleItemPI;
typedef xsltStyleItemPI *xsltStyleItemPIPtr;

struct _xsltStyleItemPI {
    XSLT_ITEM_COMMON_FIELDS
    const xmlChar *name;
    int      has_name;
};

/**
 * xsltStyleItemApplyImports:
 *
 * <!-- Category: instruction -->
 * <xsl:apply-imports />
 */
typedef xsltStyleBasicEmptyItem xsltStyleItemApplyImports;
typedef xsltStyleItemApplyImports *xsltStyleItemApplyImportsPtr;

/**
 * xsltStyleItemApplyTemplates:
 *
 * <!-- Category: instruction -->
 *  <xsl:apply-templates
 *  select = node-set-expression
 *  mode = qname>
 *  <!-- Content: (xsl:sort | xsl:with-param)* -->
 * </xsl:apply-templates>
 */
typedef struct _xsltStyleItemApplyTemplates xsltStyleItemApplyTemplates;
typedef xsltStyleItemApplyTemplates *xsltStyleItemApplyTemplatesPtr;

struct _xsltStyleItemApplyTemplates {
   XSLT_ITEM_COMMON_FIELDS

    const xmlChar *mode;	/* apply-templates */
    const xmlChar *modeURI;	/* apply-templates */
    const xmlChar *select;	/* sort, copy-of, value-of, apply-templates */
    xmlXPathCompExprPtr comp;	/* a precompiled XPath expression */
    /* TODO: with-params */
};

/**
 * xsltStyleItemCallTemplate:
 *
 * <!-- Category: instruction -->
 *  <xsl:call-template
 *  name = qname>
 *  <!-- Content: xsl:with-param* -->
 * </xsl:call-template>
 */
typedef struct _xsltStyleItemCallTemplate xsltStyleItemCallTemplate;
typedef xsltStyleItemCallTemplate *xsltStyleItemCallTemplatePtr;

struct _xsltStyleItemCallTemplate {
    XSLT_ITEM_COMMON_FIELDS

    xsltTemplatePtr templ;	/* call-template */
    const xmlChar *name;	/* element, attribute, pi */
    int      has_name;		/* element, attribute, pi */
    const xmlChar *ns;		/* element */
    int      has_ns;		/* element */
    /* TODO: with-params */
};

/**
 * xsltStyleItemCopy:
 *
 * <!-- Category: instruction -->
 * <xsl:copy
 *  use-attribute-sets = qnames>
 *  <!-- Content: template -->
 * </xsl:copy>
 */
typedef struct _xsltStyleItemCopy xsltStyleItemCopy;
typedef xsltStyleItemCopy *xsltStyleItemCopyPtr;

struct _xsltStyleItemCopy {
   XSLT_ITEM_COMMON_FIELDS
    const xmlChar *use;		/* copy, element */
    int      has_use;		/* copy, element */    
};

/**
 * xsltStyleItemIf:
 *
 * <!-- Category: instruction -->
 *  <xsl:if
 *  test = boolean-expression>
 *  <!-- Content: template -->
 * </xsl:if>
 */
typedef struct _xsltStyleItemIf xsltStyleItemIf;
typedef xsltStyleItemIf *xsltStyleItemIfPtr;

struct _xsltStyleItemIf {
    XSLT_ITEM_COMMON_FIELDS

    const xmlChar *test;	/* if */
    xmlXPathCompExprPtr comp;	/* a precompiled XPath expression */
};


/**
 * xsltStyleItemCopyOf:
 *
 * <!-- Category: instruction -->
 * <xsl:copy-of
 *  select = expression />
 */
typedef xsltStyleBasicExpressionItem xsltStyleItemCopyOf;
typedef xsltStyleItemCopyOf *xsltStyleItemCopyOfPtr;

/**
 * xsltStyleItemValueOf:
 *
 * <!-- Category: instruction -->
 * <xsl:value-of
 *  select = string-expression
 *  disable-output-escaping = "yes" | "no" />
 */
typedef struct _xsltStyleItemValueOf xsltStyleItemValueOf;
typedef xsltStyleItemValueOf *xsltStyleItemValueOfPtr;

struct _xsltStyleItemValueOf {
    XSLT_ITEM_COMMON_FIELDS

    const xmlChar *select;
    xmlXPathCompExprPtr comp;	/* a precompiled XPath expression */
    int      noescape;
};

/**
 * xsltStyleItemNumber:
 *
 * <!-- Category: instruction -->
 *  <xsl:number
 *  level = "single" | "multiple" | "any"
 *  count = pattern
 *  from = pattern
 *  value = number-expression
 *  format = { string }
 *  lang = { nmtoken }
 *  letter-value = { "alphabetic" | "traditional" }
 *  grouping-separator = { char }
 *  grouping-size = { number } />
 */
typedef struct _xsltStyleItemNumber xsltStyleItemNumber;
typedef xsltStyleItemNumber *xsltStyleItemNumberPtr;

struct _xsltStyleItemNumber {
    XSLT_ITEM_COMMON_FIELDS
    xsltNumberData numdata;	/* number */
};

/**
 * xsltStyleItemChoose:
 *
 * <!-- Category: instruction -->
 *  <xsl:choose>
 *  <!-- Content: (xsl:when+, xsl:otherwise?) -->
 * </xsl:choose>
 */
typedef xsltStyleBasicEmptyItem xsltStyleItemChoose;
typedef xsltStyleItemChoose *xsltStyleItemChoosePtr;

/**
 * xsltStyleItemFallback:
 *
 * <!-- Category: instruction -->
 *  <xsl:fallback>
 *  <!-- Content: template -->
 * </xsl:fallback>
 */
typedef xsltStyleBasicEmptyItem xsltStyleItemFallback;
typedef xsltStyleItemFallback *xsltStyleItemFallbackPtr;

/**
 * xsltStyleItemForEach:
 *
 * <!-- Category: instruction -->
 * <xsl:for-each
 *   select = node-set-expression>
 *   <!-- Content: (xsl:sort*, template) -->
 * </xsl:for-each>
 */
typedef xsltStyleBasicExpressionItem xsltStyleItemForEach;
typedef xsltStyleItemForEach *xsltStyleItemForEachPtr;

/**
 * xsltStyleItemMessage:
 *
 * <!-- Category: instruction -->
 * <xsl:message
 *   terminate = "yes" | "no">
 *   <!-- Content: template -->
 * </xsl:message>
 */
typedef struct _xsltStyleItemMessage xsltStyleItemMessage;
typedef xsltStyleItemMessage *xsltStyleItemMessagePtr;

struct _xsltStyleItemMessage {
    XSLT_ITEM_COMMON_FIELDS    
    int terminate;
};

/**
 * xsltStyleItemDocument:
 *
 * NOTE: This is not an instruction of XSLT 1.0.
 */
typedef struct _xsltStyleItemDocument xsltStyleItemDocument;
typedef xsltStyleItemDocument *xsltStyleItemDocumentPtr;

struct _xsltStyleItemDocument {
    XSLT_ITEM_COMMON_FIELDS
    int      ver11;		/* assigned: in xsltDocumentComp;
                                  read: nowhere;
                                  TODO: Check if we need. */
    const xmlChar *filename;	/* document URL */
    int has_filename;
};   

/************************************************************************
 *									*
 * Non-instructions (actually properties of instructions/declarations)  *
 *									*
 ************************************************************************/

/**
 * xsltStyleBasicItemVariable:
 *
 * Basic struct for xsl:variable, xsl:param and xsl:with-param.
 * It's currently important to have equal fields, since
 * xsltParseStylesheetCallerParam() is used with xsl:with-param from
 * the xslt side and with xsl:param from the exslt side (in
 * exsltFuncFunctionFunction()).
 *
 * FUTURE NOTE: In XSLT 2.0 xsl:param, xsl:variable and xsl:with-param
 *   have additional different fields.
 */
typedef struct _xsltStyleBasicItemVariable xsltStyleBasicItemVariable;
typedef xsltStyleBasicItemVariable *xsltStyleBasicItemVariablePtr;

struct _xsltStyleBasicItemVariable {
    XSLT_ITEM_COMMON_FIELDS

    const xmlChar *select;
    xmlXPathCompExprPtr comp;

    const xmlChar *name;
    int      has_name;
    const xmlChar *ns;
    int      has_ns;
};

/**
 * xsltStyleItemVariable:
 *
 * <!-- Category: top-level-element -->
 * <xsl:param
 *   name = qname
 *   select = expression>
 *   <!-- Content: template -->
 * </xsl:param>
 */
typedef xsltStyleBasicItemVariable xsltStyleItemVariable;
typedef xsltStyleItemVariable *xsltStyleItemVariablePtr;

/**
 * xsltStyleItemParam:
 *
 * <!-- Category: top-level-element -->
 * <xsl:param
 *   name = qname
 *   select = expression>
 *   <!-- Content: template -->
 * </xsl:param>
 */
typedef xsltStyleBasicItemVariable xsltStyleItemParam;
typedef xsltStyleItemParam *xsltStyleItemParamPtr;

/**
 * xsltStyleItemWithParam:
 *
 * <xsl:with-param
 *  name = qname
 *  select = expression>
 *  <!-- Content: template -->
 * </xsl:with-param>
 */
typedef xsltStyleBasicItemVariable xsltStyleItemWithParam;
typedef xsltStyleItemWithParam *xsltStyleItemWithParamPtr;

/**
 * xsltStyleItemSort:
 *
 * Reflects the XSLT xsl:sort item.
 * Allowed parents: xsl:apply-templates, xsl:for-each
 * <xsl:sort
 *   select = string-expression
 *   lang = { nmtoken }
 *   data-type = { "text" | "number" | qname-but-not-ncname }
 *   order = { "ascending" | "descending" }
 *   case-order = { "upper-first" | "lower-first" } />
 */
typedef struct _xsltStyleItemSort xsltStyleItemSort;
typedef xsltStyleItemSort *xsltStyleItemSortPtr;

struct _xsltStyleItemSort {
    XSLT_ITEM_COMMON_FIELDS

    const xmlChar *stype;       /* sort */
    int      has_stype;		/* sort */
    int      number;		/* sort */
    const xmlChar *order;	/* sort */
    int      has_order;		/* sort */
    int      descending;	/* sort */
    const xmlChar *lang;	/* sort */
    int      has_lang;		/* sort */
    const xmlChar *case_order;	/* sort */
    int      lower_first;	/* sort */

    const xmlChar *use;
    int      has_use;

    const xmlChar *select;	/* sort, copy-of, value-of, apply-templates */

    xmlXPathCompExprPtr comp;	/* a precompiled XPath expression */
};


/**
 * xsltStyleItemWhen:
 * 
 * <xsl:when
 *   test = boolean-expression>
 *   <!-- Content: template -->
 * </xsl:when>
 * Allowed parent: xsl:choose
 */
typedef struct _xsltStyleItemWhen xsltStyleItemWhen;
typedef xsltStyleItemWhen *xsltStyleItemWhenPtr;

struct _xsltStyleItemWhen {
    XSLT_ITEM_COMMON_FIELDS

    const xmlChar *test;
    xmlXPathCompExprPtr comp;
};

/**
 * xsltStyleItemOtherwise:
 *
 * Allowed parent: xsl:choose
 * <xsl:otherwise>
 *   <!-- Content: template -->
 * </xsl:otherwise>
 */
typedef struct _xsltStyleItemOtherwise xsltStyleItemOtherwise;
typedef xsltStyleItemOtherwise *xsltStyleItemOtherwisePtr;

struct _xsltStyleItemOtherwise {
    XSLT_ITEM_COMMON_FIELDS
};

/*
 * Literal result elements.
 * TODO: Not used yet.
 */
typedef struct _xsltStyleItemLRE xsltStyleItemLRE;
typedef xsltStyleItemLRE *xsltStyleItemLREPtr;
struct _xsltStyleItemLRE {
    XSLT_ITEM_COMMON_FIELDS
};

/************************************************************************
 *									*
 *  Compile-time structures for *internal* use only                     *
 *									*
 ************************************************************************/

/**
 * xsltCompilerNodeInfo:
 *
 * Per-node information during compile-time.
 */
typedef struct _xsltCompilerNodeInfo xsltCompilerNodeInfo;
typedef xsltCompilerNodeInfo *xsltCompilerNodeInfoPtr;
struct _xsltCompilerNodeInfo {
    xsltCompilerNodeInfoPtr next;
    xsltCompilerNodeInfoPtr prev;
    xmlNodePtr node;
    int depth;
    xsltNsListPtr inScopeNS; /* The in-scope namespaces for the current
                                position in the node-tree */
    xsltTemplatePtr templ;   /* The owning template */
    xsltElemPreCompPtr item; /* The compiled information */
};

#define XSLT_CCTXT(style) ((xsltCompilerCtxtPtr) style->compCtxt)

typedef struct _xsltCompilerCtxt xsltCompilerCtxt;
typedef xsltCompilerCtxt *xsltCompilerCtxtPtr;
struct _xsltCompilerCtxt {
    void *errorCtxt;             /* user specific error context */
    int warnings;		/* TODO: number of warnings found at
                                   compilation */
    int errors;			/* TODO: number of errors found at
                                   compilation */
    xsltStylesheetPtr sheet;
    /* TODO: structured/unstructured error contexts. */
    int depth; /* TODO: current depth in the stylesheets node-tree */
    
    xsltCompilerNodeInfoPtr inode;
    xsltCompilerNodeInfoPtr inodeList;
    xsltCompilerNodeInfoPtr inodeLast;
};   

#else /* XSLT_REFACTORED */
/*
* The old structures before refactoring.
*/

/**
 * _xsltStylePreComp:
 *
 * The in-memory structure corresponding to XSLT stylesheet constructs
 * precomputed data.
 */
struct _xsltStylePreComp {
    xsltElemPreCompPtr next;	/* chained list */
    xsltStyleType type;		/* type of the element */
    xsltTransformFunction func; /* handling function */
    xmlNodePtr inst;		/* the instruction */

    /*
     * Pre computed values.
     */

    const xmlChar *stype;       /* sort */
    int      has_stype;		/* sort */
    int      number;		/* sort */
    const xmlChar *order;	/* sort */
    int      has_order;		/* sort */
    int      descending;	/* sort */
    const xmlChar *lang;	/* sort */
    int      has_lang;		/* sort */
    const xmlChar *case_order;	/* sort */
    int      lower_first;	/* sort */

    const xmlChar *use;		/* copy, element */
    int      has_use;		/* copy, element */

    int      noescape;		/* text */

    const xmlChar *name;	/* element, attribute, pi */
    int      has_name;		/* element, attribute, pi */
    const xmlChar *ns;		/* element */
    int      has_ns;		/* element */

    const xmlChar *mode;	/* apply-templates */
    const xmlChar *modeURI;	/* apply-templates */

    const xmlChar *test;	/* if */

    xsltTemplatePtr templ;	/* call-template */

    const xmlChar *select;	/* sort, copy-of, value-of, apply-templates */

    int      ver11;		/* document */
    const xmlChar *filename;	/* document URL */
    int      has_filename;	/* document */

    xsltNumberData numdata;	/* number */

    xmlXPathCompExprPtr comp;	/* a precompiled XPath expression */
    xmlNsPtr *nsList;		/* the namespaces in scope */
    int nsNr;			/* the number of namespaces in scope */
};

#endif /* XSLT_REFACTORED */

/*
 * The in-memory structure corresponding to an XSLT Variable
 * or Param.
 */
typedef struct _xsltStackElem xsltStackElem;
typedef xsltStackElem *xsltStackElemPtr;
struct _xsltStackElem {
    struct _xsltStackElem *next;/* chained list */
    xsltStylePreCompPtr comp;   /* the compiled form */
    int computed;		/* was the evaluation done */
    const xmlChar *name;	/* the local part of the name QName */
    const xmlChar *nameURI;	/* the URI part of the name QName */
    const xmlChar *select;	/* the eval string */
    xmlNodePtr tree;		/* the tree if no eval string or the location */
    xmlXPathObjectPtr value;	/* The value if computed */
};

/*
 * TODO: We need a field to anchor an stylesheet compilation context, since,
 *   due to historical reasons, various compile-time function take only the
 *   stylesheet as argument and not a compilation context.
 */
struct _xsltStylesheet {
    /*
     * The stylesheet import relation is kept as a tree.
     */
    struct _xsltStylesheet *parent;
    struct _xsltStylesheet *next;
    struct _xsltStylesheet *imports;

    xsltDocumentPtr docList;		/* the include document list */

    /*
     * General data on the style sheet document.
     */
    xmlDocPtr doc;		/* the parsed XML stylesheet */
    xmlHashTablePtr stripSpaces;/* the hash table of the strip-space and
				   preserve space elements */
    int             stripAll;	/* strip-space * (1) preserve-space * (-1) */
    xmlHashTablePtr cdataSection;/* the hash table of the cdata-section */

    /*
     * Global variable or parameters.
     */
    xsltStackElemPtr variables; /* linked list of param and variables */

    /*
     * Template descriptions.
     */
    xsltTemplatePtr templates;	/* the ordered list of templates */
    void *templatesHash;	/* hash table or wherever compiled templates
				   informations are stored */
    void *rootMatch;		/* template based on / */
    void *keyMatch;		/* template based on key() */
    void *elemMatch;		/* template based on * */
    void *attrMatch;		/* template based on @* */
    void *parentMatch;		/* template based on .. */
    void *textMatch;		/* template based on text() */
    void *piMatch;		/* template based on processing-instruction() */
    void *commentMatch;		/* template based on comment() */
    
    /*
     * Namespace aliases.
     */
    xmlHashTablePtr nsAliases;	/* the namespace alias hash tables */

    /*
     * Attribute sets.
     */
    xmlHashTablePtr attributeSets;/* the attribute sets hash tables */

    /*
     * Namespaces.
     */
    xmlHashTablePtr nsHash;     /* the set of namespaces in use */
    void           *nsDefs;     /* the namespaces defined */

    /*
     * Key definitions.
     */
    void *keys;				/* key definitions */

    /*
     * Output related stuff.
     */
    xmlChar *method;		/* the output method */
    xmlChar *methodURI;		/* associated namespace if any */
    xmlChar *version;		/* version string */
    xmlChar *encoding;		/* encoding string */
    int omitXmlDeclaration;     /* omit-xml-declaration = "yes" | "no" */

    /* 
     * Number formatting.
     */
    xsltDecimalFormatPtr decimalFormat;
    int standalone;             /* standalone = "yes" | "no" */
    xmlChar *doctypePublic;     /* doctype-public string */
    xmlChar *doctypeSystem;     /* doctype-system string */
    int indent;			/* should output being indented */
    xmlChar *mediaType;		/* media-type string */

    /*
     * Precomputed blocks.
     */
    xsltElemPreCompPtr preComps;/* list of precomputed blocks */
    int warnings;		/* number of warnings found at compilation */
    int errors;			/* number of errors found at compilation */

    xmlChar  *exclPrefix;	/* last excluded prefixes */
    xmlChar **exclPrefixTab;	/* array of excluded prefixes */
    int       exclPrefixNr;	/* number of excluded prefixes in scope */
    int       exclPrefixMax;	/* size of the array */

    void     *_private;		/* user defined data */

    /*
     * Extensions.
     */
    xmlHashTablePtr extInfos;	/* the extension data */
    int		    extrasNr;	/* the number of extras required */

    /*
     * For keeping track of nested includes
     */
    xsltDocumentPtr includes;	/* points to last nested include */

    /*
     * dictionnary: shared between stylesheet, context and documents.
     */
    xmlDictPtr dict;
    /*
     * precompiled attribute value templates.
     */
    void *attVTs;
    /*
     * if namespace-alias has an alias for the default stylesheet prefix
     */
    const xmlChar *defaultAlias;
    /*
     * bypass pre-processing (already done) (used in imports)
     */
    int nopreproc;
    /*
     * all document text strings were internalized
     */
    int internalized;
    /*
     * Literal Result Element as Stylesheet c.f. section 2.3
     */
    int literal_result;
#ifdef XSLT_REFACTORED
    /*
    * Compilation context used during compile-time.
    */
    void * compCtxt;
    /*
    * Namespace lists.
    */
    void *inScopeNamespaces;    
#endif    
};

/*
 * The in-memory structure corresponding to an XSLT Transformation.
 */
typedef enum {
    XSLT_OUTPUT_XML = 0,
    XSLT_OUTPUT_HTML,
    XSLT_OUTPUT_TEXT
} xsltOutputType;

typedef enum {
    XSLT_STATE_OK = 0,
    XSLT_STATE_ERROR,
    XSLT_STATE_STOPPED
} xsltTransformState;

struct _xsltTransformContext {
    xsltStylesheetPtr style;		/* the stylesheet used */
    xsltOutputType type;		/* the type of output */

    xsltTemplatePtr  templ;		/* the current template */
    int              templNr;		/* Nb of templates in the stack */
    int              templMax;		/* Size of the templtes stack */
    xsltTemplatePtr *templTab;		/* the template stack */

    xsltStackElemPtr  vars;		/* the current variable list */
    int               varsNr;		/* Nb of variable list in the stack */
    int               varsMax;		/* Size of the variable list stack */
    xsltStackElemPtr *varsTab;		/* the variable list stack */
    int               varsBase;		/* the var base for current templ */

    /*
     * Extensions
     */
    xmlHashTablePtr   extFunctions;	/* the extension functions */
    xmlHashTablePtr   extElements;	/* the extension elements */
    xmlHashTablePtr   extInfos;		/* the extension data */

    const xmlChar *mode;		/* the current mode */
    const xmlChar *modeURI;		/* the current mode URI */

    xsltDocumentPtr docList;		/* the document list */

    xsltDocumentPtr document;		/* the current document */
    xmlNodePtr node;			/* the current node being processed */
    xmlNodeSetPtr nodeList;		/* the current node list */
    /* xmlNodePtr current;			the node */

    xmlDocPtr output;			/* the resulting document */
    xmlNodePtr insert;			/* the insertion node */

    xmlXPathContextPtr xpathCtxt;	/* the XPath context */
    xsltTransformState state;		/* the current state */

    /*
     * Global variables
     */
    xmlHashTablePtr   globalVars;	/* the global variables and params */

    xmlNodePtr inst;			/* the instruction in the stylesheet */

    int xinclude;			/* should XInclude be processed */

    const char *      outputFile;	/* the output URI if known */

    int profile;                        /* is this run profiled */
    long             prof;		/* the current profiled value */
    int              profNr;		/* Nb of templates in the stack */
    int              profMax;		/* Size of the templtaes stack */
    long            *profTab;		/* the profile template stack */

    void            *_private;		/* user defined data */

    int              extrasNr;		/* the number of extras used */
    int              extrasMax;		/* the number of extras allocated */
    xsltRuntimeExtraPtr extras;		/* extra per runtime informations */

    xsltDocumentPtr  styleList;		/* the stylesheet docs list */
    void                 * sec;		/* the security preferences if any */

    xmlGenericErrorFunc  error;		/* a specific error handler */
    void              * errctx;		/* context for the error handler */

    xsltSortFunc      sortfunc;		/* a ctxt specific sort routine */

    /*
     * handling of temporary Result Value Tree
     */
    xmlDocPtr       tmpRVT;		/* list of RVT without persistance */
    xmlDocPtr       persistRVT;		/* list of persistant RVTs */
    int             ctxtflags;          /* context processing flags */

    /*
     * Speed optimization when coalescing text nodes
     */
    const xmlChar  *lasttext;		/* last text node content */
    unsigned int    lasttsize;		/* last text node size */
    unsigned int    lasttuse;		/* last text node use */
    /*
     * Per Context Debugging
     */
    int debugStatus;			/* the context level debug status */
    unsigned long* traceCode;		/* pointer to the variable holding the mask */

    int parserOptions;			/* parser options xmlParserOption */

    /*
     * dictionnary: shared between stylesheet, context and documents.
     */
    xmlDictPtr dict;
    /*
     * temporary storage for doc ptr, currently only used for
     * global var evaluation
     */
    xmlDocPtr		tmpDoc;
    /*
     * all document text strings are internalized
     */
    int internalized;
};

/**
 * CHECK_STOPPED:
 *
 * Macro to check if the XSLT processing should be stopped.
 * Will return from the function.
 */
#define CHECK_STOPPED if (ctxt->state == XSLT_STATE_STOPPED) return;

/**
 * CHECK_STOPPEDE:
 *
 * Macro to check if the XSLT processing should be stopped.
 * Will goto the error: label.
 */
#define CHECK_STOPPEDE if (ctxt->state == XSLT_STATE_STOPPED) goto error;

/**
 * CHECK_STOPPED0:
 *
 * Macro to check if the XSLT processing should be stopped.
 * Will return from the function with a 0 value.
 */
#define CHECK_STOPPED0 if (ctxt->state == XSLT_STATE_STOPPED) return(0);

/*
 * The macro XML_CAST_FPTR is a hack to avoid a gcc warning about
 * possible incompatibilities between function pointers and object
 * pointers.  It is defined in libxml/hash.h within recent versions
 * of libxml2, but is put here for compatibility.
 */
#ifndef XML_CAST_FPTR
/**
 * XML_CAST_FPTR:
 * @fptr:  pointer to a function
 *
 * Macro to do a casting from an object pointer to a
 * function pointer without encountering a warning from
 * gcc
 *
 * #define XML_CAST_FPTR(fptr) (*(void **)(&fptr))
 * This macro violated ISO C aliasing rules (gcc4 on s390 broke)
 * so it is disabled now
 */

#define XML_CAST_FPTR(fptr) fptr
#endif
/*
 * Functions associated to the internal types
xsltDecimalFormatPtr	xsltDecimalFormatGetByName(xsltStylesheetPtr sheet,
						   xmlChar *name);
 */
XSLTPUBFUN xsltStylesheetPtr XSLTCALL	
			xsltNewStylesheet	(void);
XSLTPUBFUN xsltStylesheetPtr XSLTCALL	
			xsltParseStylesheetFile	(const xmlChar* filename);
XSLTPUBFUN void XSLTCALL			
			xsltFreeStylesheet	(xsltStylesheetPtr sheet);
XSLTPUBFUN int XSLTCALL			
			xsltIsBlank		(xmlChar *str);
XSLTPUBFUN void XSLTCALL			
			xsltFreeStackElemList	(xsltStackElemPtr elem);
XSLTPUBFUN xsltDecimalFormatPtr XSLTCALL	
			xsltDecimalFormatGetByName(xsltStylesheetPtr sheet,
						 xmlChar *name);

XSLTPUBFUN xsltStylesheetPtr XSLTCALL	
			xsltParseStylesheetProcess(xsltStylesheetPtr ret,
						 xmlDocPtr doc);
XSLTPUBFUN void XSLTCALL			
			xsltParseStylesheetOutput(xsltStylesheetPtr style,
						 xmlNodePtr cur);
XSLTPUBFUN xsltStylesheetPtr XSLTCALL	
			xsltParseStylesheetDoc	(xmlDocPtr doc);
XSLTPUBFUN xsltStylesheetPtr XSLTCALL	
			xsltParseStylesheetImportedDoc(xmlDocPtr doc,
						xsltStylesheetPtr style);
XSLTPUBFUN xsltStylesheetPtr XSLTCALL	
			xsltLoadStylesheetPI	(xmlDocPtr doc);
XSLTPUBFUN void XSLTCALL 			
			xsltNumberFormat	(xsltTransformContextPtr ctxt,
						 xsltNumberDataPtr data,
						 xmlNodePtr node);
XSLTPUBFUN xmlXPathError XSLTCALL		 
			xsltFormatNumberConversion(xsltDecimalFormatPtr self,
						 xmlChar *format,
						 double number,
						 xmlChar **result);

XSLTPUBFUN void XSLTCALL			
			xsltParseTemplateContent(xsltStylesheetPtr style,
						 xmlNodePtr templ);
XSLTPUBFUN int XSLTCALL			
			xsltAllocateExtra	(xsltStylesheetPtr style);
XSLTPUBFUN int XSLTCALL			
			xsltAllocateExtraCtxt	(xsltTransformContextPtr ctxt);
/*
 * Extra functions for Result Value Trees
 */
XSLTPUBFUN xmlDocPtr XSLTCALL		
			xsltCreateRVT		(xsltTransformContextPtr ctxt);
XSLTPUBFUN int XSLTCALL			
			xsltRegisterTmpRVT	(xsltTransformContextPtr ctxt,
						 xmlDocPtr RVT);
XSLTPUBFUN int XSLTCALL			
			xsltRegisterPersistRVT	(xsltTransformContextPtr ctxt,
						 xmlDocPtr RVT);
XSLTPUBFUN void XSLTCALL			
			xsltFreeRVTs		(xsltTransformContextPtr ctxt);
			
/*
 * Extra functions for Attribute Value Templates
 */
XSLTPUBFUN void XSLTCALL
			xsltCompileAttr		(xsltStylesheetPtr style,
						 xmlAttrPtr attr);
XSLTPUBFUN xmlChar * XSLTCALL
			xsltEvalAVT		(xsltTransformContextPtr ctxt,
						 void *avt,
						 xmlNodePtr node);
XSLTPUBFUN void XSLTCALL
			xsltFreeAVTList		(void *avt);

/************************************************************************
 *									*
 *  Compile-time functions for *internal* use only                      *
 *									*
 ************************************************************************/

#ifdef XSLT_REFACTORED
XSLTPUBFUN xsltNsListPtr XSLTCALL
			xsltCompilerGetInScopeNSInfo(xsltCompilerCtxtPtr cctxt,
						     xmlNodePtr node);
#endif /* XSLT_REFACTORED */
/*
 * Extra function for successful xsltCleanupGlobals / xsltInit sequence.
 */

XSLTPUBFUN void XSLTCALL
			xsltUninit		(void);

#ifdef __cplusplus
}
#endif

#endif /* __XML_XSLT_H__ */

