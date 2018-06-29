#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofBackground(239);
	ofSetWindowTitle("Insta");

	ofSetRectMode(ofRectMode::OF_RECTMODE_CENTER);

	this->fbo.allocate(ofGetWidth(), ofGetHeight());
	this->fbo.readToPixels(this->pixels);
	this->pixels_mat = cv::Mat(this->pixels.getHeight(), this->pixels.getWidth(), CV_8UC4, this->pixels.getData());
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	this->fbo.begin();
	ofClear(0);
	ofTranslate(this->fbo.getWidth() * 0.5, this->fbo.getHeight() * 0.5);

	ofSetColor(59, 59, 239);
	int radius = 250;
	ofBeginShape();
	for (int deg = ofGetFrameNum() * 2; deg <= ofGetFrameNum() * 2 + 360; deg += 120) {

		ofVertex(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD));
	}

	ofNextContour();
	radius *= 0.7;
	for (int deg = ofGetFrameNum() * 2; deg <= ofGetFrameNum() * 2 + 360; deg += 120) {

		ofVertex(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD));
	}
	ofEndShape(true);

	this->fbo.end();
	this->fbo.readToPixels(this->pixels);

	cv::Mat gray_mat;
	cv::cvtColor(this->pixels_mat, gray_mat, cv::COLOR_RGBA2GRAY);
	for (int y = 0; y < this->pixels_mat.cols; y++) {

		for (int x = 0; x < this->pixels_mat.rows; x++) {

			unsigned char value = gray_mat.at<unsigned char>(y, x);
			if (value > 0) {

				this->pixels_mat.at<cv::Vec4b>(y, x) = cv::Vec4b(value, value, value, this->pixels_mat.at<cv::Vec4b>(y, x)[3]);
			}
		}
	}

	cv::GaussianBlur(this->pixels_mat, this->pixels_mat, cv::Size(19, 19), 5, 5);
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofTranslate(ofGetWidth() * 0.5, ofGetHeight() * 0.5);

	ofPoint point = this->get_accelerometer_3d();

	ofSetColor(255, 192);
	ofImage draw_image;
	draw_image.setFromPixels(this->pixels);
	draw_image.draw(ofMap(point.x, -1, 1, -100, 100), ofMap(point.y, -1, 1, 100, -100));

	ofSetColor(255);
	this->fbo.draw(0, 0);
}

//--------------------------------------------------------------
ofPoint ofApp::get_accelerometer_3d() {

	CComPtr<ISensorManager> sensor_manager;
	CComPtr<ISensorCollection> sensor_collection;
	CComPtr<ISensor> sensor;
	CComPtr<ISensorDataReport> data;

	if (FAILED(::CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return ofPoint(); }
	if (FAILED(::CoCreateInstance(CLSID_SensorManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&sensor_manager)))) { return ofPoint(); }
	if (FAILED(sensor_manager->GetSensorsByCategory(SENSOR_TYPE_ACCELEROMETER_3D, &sensor_collection))) { return ofPoint(); }
	if (FAILED(sensor_collection->GetAt(0, &sensor))) { return ofPoint(); }
	if (FAILED(sensor->GetData(&data))) { return ofPoint(); }

	PROPVARIANT x = {};
	if (FAILED(data->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_X_G, &x))) { return ofPoint(); }
	PROPVARIANT y = {};
	if (FAILED(data->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_Y_G, &y))) { return ofPoint(); }
	PROPVARIANT z = {};
	if (FAILED(data->GetSensorValue(SENSOR_DATA_TYPE_ACCELERATION_Z_G, &z))) { return ofPoint(); }

	return ofPoint(x.dblVal, y.dblVal, z.dblVal);
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}