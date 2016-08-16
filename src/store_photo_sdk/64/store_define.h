#ifndef __STORE_PHOTO_DEFINE_H
#define __STORE_PHOTO_DEFINE_H

#include<string>
#include<vector>
#include<map>
#include<time.h>
#include<stdio.h>

using namespace std;

namespace store_photo_sdk
{

//上传附加可选参数
struct upload_param{
	unsigned savedays;        //文件存储天数,0表示永久存储
	unsigned clientip;        //客户端IP.
	vector<string> notify;    //用于主动推送(就近下载)的信息。
	string secure_info;       //安全审核用的业务特别信息，大部分业务无需考虑
    unsigned need_rsp_data;

	upload_param(){
		savedays = 0;
		clientip = 0;
		secure_info = "";
        need_rsp_data = 0;
	}
};

#pragma pack(1)
struct info_head_t{
    char ver;
	unsigned fsize;
	unsigned chid;  
	unsigned fid;   
	unsigned buildtime;  
	unsigned updatetime; 
	char num;        
	int off_a[0];
};      
#pragma pack()			

			
struct multi_info_t{
	string inter_info;      //存储平台侧的文件属性（比如文件存储大小、创建时间、更新时间等）
	string file_info;       //业务填写的文件信息
};
														  

/* 对于各种拉列表操作，存储的输出参数 */
struct result_t
{
    string uuid;			//文件id
    string info;            //文件信息
};

struct result_list
{
    vector<result_t> list;
};

struct result_file_t
{
    string uuid;           //文件id
	multi_info_t info;     //文件信息（业务填写的信息及存储平台侧的文件属性）
};

struct result_file_list
{
	vector<result_file_t> list;
};


/* 压缩规格 */
struct scale_size_t
{
	unsigned width;
	unsigned height;
};

struct scale_size_list
{
	vector<scale_size_t> list;
};

/* 上传成功后返回的图片信息(各个尺寸分别的信息，以一个列表形式返回） */
struct pic_info_t
{
    unsigned tag;
    unsigned width;
    unsigned height;
	unsigned fsize;       //图片大小
	string fmd5;          //图片md5值
    string data;
};

struct pic_info_list
{
	vector<pic_info_t> list;
};

/* 预上传参数list */
struct tryupload_t
{
	unsigned uin;
	string parentid;
	string fileid;
	string file_info;
	string data_md5;
	string secure_info;
	unsigned data_len;
	unsigned savedays;
	int retcode;
	unsigned chid;
	unsigned fid ;
};

struct tryupload_list
{
	vector<tryupload_t> list;
};

/* 生成目录名，形式为: 年份w月份，示例： 2010w07 */
static inline void generate_folderid(string &folderid)
{
	time_t timep;
	time(&timep);
	struct tm *now_tm;
	now_tm = gmtime(&timep);

	int mday = now_tm->tm_mday;
	int week = (mday - 1)/7 + 1;

	char tmp_id[10];
	snprintf(tmp_id, 10, "%d%02dw%02d", 1900 + now_tm->tm_year, 1 + now_tm->tm_mon, week);

	folderid.assign(tmp_id, 9);

}

const char exif[][50]={
	"origin_width",
	"origin_height",
	"origin_type",
	"origin_size",
	"frame_count",
    "bgcolor",
	"exif:InteroperabilityVersion",
	"exif:ImageWidth",
    "exif:ImageLength",
    "exif:BitsPerSample",
    "exif:Compression",
    "exif:PhotometricInterpretation",
    "exif:FillOrder",
    "exif:DocumentName",
    "exif:ImageDescription",
    "exif:Make" ,
    "exif:Model" ,
    "exif:StripOffsets", 
    "exif:Orientation" ,
	"exif:SamplesPerPixel" ,
	"exif:RowsPerStrip" ,
	"exif:StripByteCounts" ,
	"exif:XResolution" ,
	"exif:YResolution" ,
	"exif:PlanarConfiguration" ,
	"exif:PageName" ,
	"exif:XPosition" ,
	"exif:YPosition" ,
	"exif:MinSampleValue" ,
	"exif:MaxSampleValue" ,
	"exif:FreeOffsets" ,
	"exif:FreeByteCounts" ,
	"exif:GrayResponseUnit" ,
	"exif:GrayResponseCurve" ,
	"exif:T4Options" ,
	"exif:T6Options" ,
	"exif:ResolutionUnit" ,
	"exif:TransferFunction" ,
	"exif:Software" ,
	"exif:DateTime" ,
	"exif:Artist" ,
	"exif:WhitePoint" ,
	"exif:PrimaryChromaticities" ,
	"exif:ColorMap" ,
	"exif:HalfToneHints" ,
	"exif:TileWidth" ,
	"exif:TileLength" ,
	"exif:TileOffsets" ,
	"exif:TileByteCounts" ,
	"exif:SubIFD" ,
	"exif:InkSet" ,
	"exif:InkNames" ,
	"exif:NumberOfInks" ,
	"exif:DotRange" ,
	"exif:TargetPrinter" ,
	"exif:ExtraSample" ,
	"exif:SampleFormat" ,
	"exif:SMinSampleValue" ,
	"exif:SMaxSampleValue" ,
	"exif:TransferRange" ,
	"exif:ClipPath" ,
	"exif:XClipPathUnits" ,
	"exif:YClipPathUnits" ,
	"exif:Indexed" ,
	"exif:JPEGTables" ,
	"exif:OPIProxy" ,
	"exif:JPEGProc" ,
	"exif:JPEGInterchangeFormat" ,
	"exif:JPEGInterchangeFormatLength" ,
	"exif:JPEGRestartInterval" ,
	"exif:JPEGLosslessPredictors" ,
	"exif:JPEGPointTransforms" ,
	"exif:JPEGQTables" ,
	"exif:JPEGDCTables" ,
	"exif:JPEGACTables" ,
	"exif:YCbCrCoefficients" ,
	"exif:YCbCrSubSampling" ,
	"exif:YCbCrPositioning" ,
	"exif:ReferenceBlackWhite" ,
	"exif:ExtensibleMetadataPlatform" ,
	"exif:Gamma" ,
	"exif:ICCProfileDescriptor" ,
	"exif:SRGBRenderingIntent" ,
	"exif:ImageTitle" ,
	"exif:ResolutionXUnit" ,
	"exif:ResolutionYUnit" ,
	"exif:ResolutionXLengthUnit" ,
	"exif:ResolutionYLengthUnit" ,
	"exif:PrintFlags" ,
	"exif:PrintFlagsVersion" ,
	"exif:PrintFlagsCrop" ,
	"exif:PrintFlagsBleedWidth" ,
	"exif:PrintFlagsBleedWidthScale" ,
	"exif:HalftoneLPI" ,
	"exif:HalftoneLPIUnit" ,
	"exif:HalftoneDegree" ,
	"exif:HalftoneShape" ,
	"exif:HalftoneMisc" ,
	"exif:HalftoneScreen" ,
	"exif:JPEGQuality" ,
	"exif:GridSize" ,
	"exif:ThumbnailFormat" ,
	"exif:ThumbnailWidth" ,
	"exif:ThumbnailHeight" ,
	"exif:ThumbnailColorDepth" ,
	"exif:ThumbnailPlanes" ,
	"exif:ThumbnailRawBytes" ,
	"exif:ThumbnailSize" ,
	"exif:ThumbnailCompressedSize" ,
	"exif:ColorTransferFunction" ,
	"exif:ThumbnailData" ,
	"exif:ThumbnailImageWidth" ,
	"exif:ThumbnailImageHeight" ,
	"exif:ThumbnailBitsPerSample" ,
	"exif:ThumbnailCompression" ,
	"exif:ThumbnailPhotometricInterp" ,
	"exif:ThumbnailImageDescription" ,
	"exif:ThumbnailEquipMake" ,
	"exif:ThumbnailEquipModel" ,
	"exif:ThumbnailStripOffsets" ,
	"exif:ThumbnailOrientation" ,
	"exif:ThumbnailSamplesPerPixel" ,
	"exif:ThumbnailRowsPerStrip" ,
	"exif:ThumbnailStripBytesCount" ,
	"exif:ThumbnailResolutionX" ,
	"exif:ThumbnailResolutionY" ,
	"exif:ThumbnailPlanarConfig" ,
	"exif:ThumbnailResolutionUnit" ,
	"exif:ThumbnailTransferFunction" ,
	"exif:ThumbnailSoftwareUsed" ,
	"exif:ThumbnailDateTime" ,
	"exif:ThumbnailArtist" ,
	"exif:ThumbnailWhitePoint" ,
	"exif:ThumbnailPrimaryChromaticities" ,
	"exif:ThumbnailYCbCrCoefficients" ,
	"exif:ThumbnailYCbCrSubsampling" ,
	"exif:ThumbnailYCbCrPositioning" ,
	"exif:ThumbnailRefBlackWhite" ,
	"exif:ThumbnailCopyRight" ,
	"exif:LuminanceTable" ,
	"exif:ChrominanceTable" ,
	"exif:FrameDelay" ,
	"exif:LoopCount" ,
	"exif:PixelUnit" ,
	"exif:PixelPerUnitX" ,
	"exif:PixelPerUnitY" ,
	"exif:PaletteHistogram" ,
	"exif:RelatedImageFileFormat" ,
	"exif:RelatedImageLength" ,
	"exif:RelatedImageWidth" ,
	"exif:ImageID" ,
	"exif:Matteing" ,
	"exif:DataType" ,
	"exif:ImageDepth" ,
	"exif:TileDepth" ,
	"exif:CFARepeatPatternDim" ,
	"exif:CFAPattern2" ,
	"exif:BatteryLevel" ,
	"exif:Copyright" ,
	"exif:ExposureTime" ,
	"exif:FNumber" ,
	"exif:IPTC/NAA" ,
	"exif:IT8RasterPadding" ,
	"exif:IT8ColorTable" ,
	"exif:ImageResourceInformation" ,
	"exif:ExifOffset" ,
	"exif:InterColorProfile" ,
	"exif:ExposureProgram" ,
	"exif:SpectralSensitivity" ,
	"exif:GPSInfo" ,
	"exif:ISOSpeedRatings" ,
	"exif:OECF" ,
	"exif:Interlace" ,
	"exif:TimeZoneOffset" ,
	"exif:SelfTimerMode" ,
	"exif:ExifVersion" ,
	"exif:DateTimeOriginal" ,
	"exif:CompressedBitsPerPixel" ,
	"exif:ShutterSpeedValue" ,
	"exif:ApertureValue" ,
	"exif:BrightnessValue" ,
	"exif:ExposureBiasValue" ,
	"exif:MaxApertureValue" ,
	"exif:SubjectDistance" ,
	"exif:MeteringMode" ,
	"exif:LightSource" ,
	"exif:Flash" ,
	"exif:FocalLength" ,
	"exif:FlashEnergy" ,
	"exif:SpatialFrequencyResponse" ,
	"exif:Noise" ,
	"exif:ImageNumber" ,
	"exif:SecurityClassification" ,
	"exif:ImageHistory" ,
	"exif:SubjectArea" ,
	"exif:ExposureIndex" ,
	"exif:TIFF-EPStandardID" ,
	"exif:MakerNote" ,
	"exif:WinXP-Title" ,
	"exif:WinXP-Comments" ,
	"exif:WinXP-Author" ,
	"exif:WinXP-Keywords" ,
	"exif:WinXP-Subject" ,
	"exif:UserComment" ,
	"exif:SubSecTime" ,
	"exif:SubSecTimeOriginal" ,
	"exif:SubSecTimeDigitized" ,
	"exif:FlashPixVersion" ,
	"exif:ColorSpace" ,
	"exif:ExifImageWidth" ,
	"exif:ExifImageLength" ,
	"exif:RelatedSoundFile" ,
	"exif:InteroperabilityOffset" ,
	"exif:FlashEnergy" ,
	"exif:SpatialFrequencyResponse" ,
	"exif:Noise" ,
	"exif:FocalPlaneXResolution" ,
	"exif:FocalPlaneYResolution" ,
	"exif:FocalPlaneResolutionUnit" ,
	"exif:SubjectLocation" ,
	"exif:ExposureIndex" ,
	"exif:TIFF/EPStandardID" ,
	"exif:SensingMethod" ,
	"exif:FileSource" ,
	"exif:SceneType" ,
	"exif:CFAPattern" ,
	"exif:CustomRendered" ,
	"exif:ExposureMode" ,
	"exif:WhiteBalance" ,
	"exif:DigitalZoomRatio" ,
	"exif:FocalLengthIn35mmFilm" ,
	"exif:SceneCaptureType" ,
	"exif:GainControl" ,
	"exif:Contrast" ,
	"exif:Saturation" ,
	"exif:Sharpness" ,
	"exif:DeviceSettingDescription" ,
	"exif:SubjectDistanceRange" ,
	"exif:ImageUniqueID" ,
	"exif:PrintImageMatching" ,
	"exif:GPSVersionID" ,
	"exif:GPSLatitudeRef" ,
	"exif:GPSLatitude" ,
	"exif:GPSLongitudeRef" ,
	"exif:GPSLongitude" ,
	"exif:GPSAltitudeRef" ,
	"exif:GPSAltitude" ,
	"exif:GPSTimeStamp" ,
	"exif:GPSSatellites" ,
	"exif:GPSStatus" ,
	"exif:GPSMeasureMode" ,
	"exif:GPSDop" ,
	"exif:GPSSpeedRef" ,
	"exif:GPSSpeed" ,
	"exif:GPSTrackRef" ,
	"exif:GPSTrack" ,
	"exif:GPSImgDirectionRef" ,
	"exif:GPSImgDirection" ,
	"exif:GPSMapDatum" ,
	"exif:GPSDestLatitudeRef" ,
	"exif:GPSDestLatitude" ,
	"exif:GPSDestLongitudeRef" ,
	"exif:GPSDestLongitude" ,
	"exif:GPSDestBearingRef" ,
	"exif:GPSDestBearing" ,
	"exif:GPSDestDistanceRef" ,
	"exif:GPSDestDistance" ,
	"exif:GPSProcessingMethod" ,
	"exif:GPSAreaInformation" ,
	"exif:GPSDateStamp" ,
	"exif:GPSDifferential"

};

//--------upp--------
enum MULTI_SPEC_TYPE
{
	COMBINE_SMALL = 1,  //- 100*100
	COMBINE_MID1  = 2,  //- 160*800
	COMBINE_MID2  = 3,  //- 200*1000
	COMBINE_MID3  = 4,  //- 400*2000
	COMBINE_BIG   = 5,  //- 670*3350,800*4000
	HD_PIC        = 6,  //- 1280,1600,1920
	RAW_PIC       = 7  
};

typedef struct specInfo
{
	uint32_t img_size;
	uint32_t img_width;
	uint32_t img_height;
	uint8_t img_type;
	specInfo()
	{
		img_size = 0;
		img_width = 0;
		img_height = 0;
		img_type = 0;
	}

}SPECINFO;
typedef struct fileNode
{
	uint32_t uin;
	uint32_t uploadtime; 
	uint32_t updatetime;
	map<MULTI_SPEC_TYPE, SPECINFO> mp_spec_info;
	fileNode()
	{
		uin = 0;
		uploadtime = 0;
		updatetime = 0;
		mp_spec_info.clear();
	}

}FILENODE;
//--------upp--------
};

#endif

