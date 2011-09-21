/****************************************************************************
** BBB_GLWidget meta object code from reading C++ file 'bbb_GLWidget.h'
**
** Created: Sat Jan 31 12:28:23 2009
**      by: The Qt MOC ($Id: moc_yacc.cpp 2051 2007-02-21 10:04:20Z chehrlic $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "bbb_GLWidget.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *BBB_GLWidget::className() const
{
    return "BBB_GLWidget";
}

QMetaObject *BBB_GLWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_BBB_GLWidget( "BBB_GLWidget", &BBB_GLWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString BBB_GLWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "BBB_GLWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString BBB_GLWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "BBB_GLWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* BBB_GLWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QGLWidget::staticMetaObject();
    static const QUMethod slot_0 = {"rebuildBricks", 0, 0 };
    static const QUMethod slot_1 = {"resizeWall", 0, 0 };
    static const QUMethod slot_2 = {"resizeColumns", 0, 0 };
    static const QUMethod slot_3 = {"resizePadding", 0, 0 };
    static const QUMethod slot_4 = {"cantileverPattern", 0, 0 };
    static const QUParameter param_slot_5[] = {
	{ "col", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_5 = {"cantileverPattern", 1, param_slot_5 };
    static const QUMethod slot_6 = {"activateBricks", 0, 0 };
    static const QUMethod slot_7 = {"clearActiveBricks", 0, 0 };
    static const QUMethod slot_8 = {"randomActiveBricks", 0, 0 };
    static const QUMethod slot_9 = {"generatePattern", 0, 0 };
    static const QUMethod slot_10 = {"clearPattern", 0, 0 };
    static const QUMethod slot_11 = {"clearRules", 0, 0 };
    static const QUMethod slot_12 = {"generateBeams", 0, 0 };
    static const QUMethod slot_13 = {"clearBeams", 0, 0 };
    static const QUParameter param_slot_14[] = {
	{ "index", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_14 = {"clearColumn", 1, param_slot_14 };
    static const QUMethod slot_15 = {"toggleGrid", 0, 0 };
    static const QUMethod slot_16 = {"toggleBorders", 0, 0 };
    static const QUMethod slot_17 = {"showInactive", 0, 0 };
    static const QUMethod slot_18 = {"toggleOffset", 0, 0 };
    static const QUMethod slot_19 = {"invertActive", 0, 0 };
    static const QUMethod slot_20 = {"zoomExtents", 0, 0 };
    static const QUMethod slot_21 = {"resetAll", 0, 0 };
    static const QUMethod slot_22 = {"takeScreenshot", 0, 0 };
    static const QUParameter param_slot_23[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out }
    };
    static const QUMethod slot_23 = {"importPath", 1, param_slot_23 };
    static const QUParameter param_slot_24[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out },
	{ "filename", &static_QUType_charstar, 0, QUParameter::In }
    };
    static const QUMethod slot_24 = {"importPath", 2, param_slot_24 };
    static const QUParameter param_slot_25[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out }
    };
    static const QUMethod slot_25 = {"savePattern", 1, param_slot_25 };
    static const QUParameter param_slot_26[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out }
    };
    static const QUMethod slot_26 = {"loadPattern", 1, param_slot_26 };
    static const QUParameter param_slot_27[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out }
    };
    static const QUMethod slot_27 = {"exportToObj", 1, param_slot_27 };
    static const QUParameter param_slot_28[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out }
    };
    static const QUMethod slot_28 = {"exportToRLE", 1, param_slot_28 };
    static const QUParameter param_slot_29[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out }
    };
    static const QUMethod slot_29 = {"exportToMP3", 1, param_slot_29 };
    static const QMetaData slot_tbl[] = {
	{ "rebuildBricks()", &slot_0, QMetaData::Public },
	{ "resizeWall()", &slot_1, QMetaData::Public },
	{ "resizeColumns()", &slot_2, QMetaData::Public },
	{ "resizePadding()", &slot_3, QMetaData::Public },
	{ "cantileverPattern()", &slot_4, QMetaData::Public },
	{ "cantileverPattern(int)", &slot_5, QMetaData::Public },
	{ "activateBricks()", &slot_6, QMetaData::Public },
	{ "clearActiveBricks()", &slot_7, QMetaData::Public },
	{ "randomActiveBricks()", &slot_8, QMetaData::Public },
	{ "generatePattern()", &slot_9, QMetaData::Public },
	{ "clearPattern()", &slot_10, QMetaData::Public },
	{ "clearRules()", &slot_11, QMetaData::Public },
	{ "generateBeams()", &slot_12, QMetaData::Public },
	{ "clearBeams()", &slot_13, QMetaData::Public },
	{ "clearColumn(int)", &slot_14, QMetaData::Public },
	{ "toggleGrid()", &slot_15, QMetaData::Public },
	{ "toggleBorders()", &slot_16, QMetaData::Public },
	{ "showInactive()", &slot_17, QMetaData::Public },
	{ "toggleOffset()", &slot_18, QMetaData::Public },
	{ "invertActive()", &slot_19, QMetaData::Public },
	{ "zoomExtents()", &slot_20, QMetaData::Public },
	{ "resetAll()", &slot_21, QMetaData::Public },
	{ "takeScreenshot()", &slot_22, QMetaData::Public },
	{ "importPath()", &slot_23, QMetaData::Public },
	{ "importPath(char*)", &slot_24, QMetaData::Public },
	{ "savePattern()", &slot_25, QMetaData::Public },
	{ "loadPattern()", &slot_26, QMetaData::Public },
	{ "exportToObj()", &slot_27, QMetaData::Public },
	{ "exportToRLE()", &slot_28, QMetaData::Public },
	{ "exportToMP3()", &slot_29, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"BBB_GLWidget", parentObject,
	slot_tbl, 30,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_BBB_GLWidget.setMetaObject( metaObj );
    return metaObj;
}

void* BBB_GLWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "BBB_GLWidget" ) )
	return this;
    return QGLWidget::qt_cast( clname );
}

bool BBB_GLWidget::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: rebuildBricks(); break;
    case 1: resizeWall(); break;
    case 2: resizeColumns(); break;
    case 3: resizePadding(); break;
    case 4: cantileverPattern(); break;
    case 5: cantileverPattern((int)static_QUType_int.get(_o+1)); break;
    case 6: activateBricks(); break;
    case 7: clearActiveBricks(); break;
    case 8: randomActiveBricks(); break;
    case 9: generatePattern(); break;
    case 10: clearPattern(); break;
    case 11: clearRules(); break;
    case 12: generateBeams(); break;
    case 13: clearBeams(); break;
    case 14: clearColumn((int)static_QUType_int.get(_o+1)); break;
    case 15: toggleGrid(); break;
    case 16: toggleBorders(); break;
    case 17: showInactive(); break;
    case 18: toggleOffset(); break;
    case 19: invertActive(); break;
    case 20: zoomExtents(); break;
    case 21: resetAll(); break;
    case 22: takeScreenshot(); break;
    case 23: static_QUType_bool.set(_o,importPath()); break;
    case 24: static_QUType_bool.set(_o,importPath((char*)static_QUType_charstar.get(_o+1))); break;
    case 25: static_QUType_bool.set(_o,savePattern()); break;
    case 26: static_QUType_bool.set(_o,loadPattern()); break;
    case 27: static_QUType_bool.set(_o,exportToObj()); break;
    case 28: static_QUType_bool.set(_o,exportToRLE()); break;
    case 29: static_QUType_bool.set(_o,exportToMP3()); break;
    default:
	return QGLWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool BBB_GLWidget::qt_emit( int _id, QUObject* _o )
{
    return QGLWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool BBB_GLWidget::qt_property( int id, int f, QVariant* v)
{
    return QGLWidget::qt_property( id, f, v);
}

bool BBB_GLWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
