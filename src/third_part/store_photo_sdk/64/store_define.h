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

//�ϴ����ӿ�ѡ����
struct upload_param{
	unsigned savedays;        //�ļ��洢����,0��ʾ���ô洢
	unsigned clientip;        //�ͻ���IP.
	vector<string> notify;    //������������(�ͽ�����)����Ϣ��
	string secure_info;       //��ȫ����õ�ҵ���ر���Ϣ���󲿷�ҵ�����迼��
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
	string inter_info;      //�洢ƽ̨����ļ����ԣ������ļ��洢��С������ʱ�䡢����ʱ��ȣ�
	string file_info;       //ҵ����д���ļ���Ϣ
};
														  

/* ���ڸ������б�������洢��������� */
struct result_t
{
    string uuid;			//�ļ�id
    string info;            //�ļ���Ϣ
};

struct result_list
{
    vector<result_t> list;
};

struct result_file_t
{
    string uuid;           //�ļ�id
	multi_info_t info;     //�ļ���Ϣ��ҵ����д����Ϣ���洢ƽ̨����ļ����ԣ�
};

struct result_file_list
{
	vector<result_file_t> list;
};


/* ѹ����� */
struct scale_size_t
{
	unsigned width;
	unsigned height;
};

struct scale_size_list
{
	vector<scale_size_t> list;
};

/* �ϴ��ɹ��󷵻ص�ͼƬ��Ϣ(�����ߴ�ֱ����Ϣ����һ���б���ʽ���أ� */
struct pic_info_t
{
    unsigned tag;
    unsigned width;
    unsigned height;
	unsigned fsize;       //ͼƬ��С
	string fmd5;          //ͼƬmd5ֵ
    string data;
};

struct pic_info_list
{
	vector<pic_info_t> list;
};

/* Ԥ�ϴ�����list */
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

/* ����Ŀ¼������ʽΪ: ���w�·ݣ�ʾ���� 2010w07 */
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

