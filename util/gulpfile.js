/*
ESPRemoteLog html compressor
Based on ESP8266 file system builder by Xose Pérez <xose dot perez at gmail dot com>
*/
 
// -----------------------------------------------------------------------------
// File system builder
// -----------------------------------------------------------------------------
 
// Intended to be run from projectDir/util/
// Looks for HTML in projectDir/html

const path = require('path');
const fs = require('fs');
const gulp = require('gulp');
const htmlmin = require('gulp-htmlmin');
const cleancss = require('gulp-clean-css');
const uglify = require('gulp-uglify');
const gzip = require('gulp-gzip');
const del = require('del');
const inline = require('gulp-inline');
const inlineImages = require('gulp-css-base64');
const favicon = require('gulp-base64-favicon');

const intName = 'log.html.gz';
const headerFileName = 'compressed_log.h';

let htmlPath = path.join(__dirname, '../html/');
let dataPath = path.join(__dirname, 'comp/');
let rootPath = path.join(__dirname, '../');
 
gulp.task('clean', function() {
    del([dataPath + '*']);
    return true;
});
 
gulp.task('buildfs_embeded', ['buildfs_inline'], function() {
 
    var source = dataPath + intName;
    var destination = rootPath + headerFileName;
 
    var wstream = fs.createWriteStream(destination);
    wstream.on('error', function (err) {
        console.log(err);
    });
 
    var data = fs.readFileSync(source);
    
    wstream.write('// Compressed log.html in gz/binary form, for inclusion in binary (no reliance on FS)\n\n');
    wstream.write('#ifndef COMPRESSED_LOG_H\n');
    wstream.write('#define COMPRESSED_LOG_H\n\n');
    wstream.write('// BINARY_START\n');
    wstream.write('#define log_html_gz_len ' + data.length + '\n');
    wstream.write('const uint8_t log_html_gz[] PROGMEM = {')

    for (i=0; i<data.length; i++) {
        if (i % 1000 == 0) wstream.write("\n");
        wstream.write('0x' + ('00' + data[i].toString(16)).slice(-2));
        if (i<data.length-1) wstream.write(',');
    }
 
    wstream.write('\n};\n')
    wstream.write('// BINARY_END\n\n');
    wstream.write('#endif');
    wstream.end();
 
    del();
 
});
 
gulp.task('buildfs_inline', ['clean'], function() {
    return gulp.src('html/*.html', {cwd:'..'})
        .pipe(favicon(htmlPath))
        .pipe(inline({
            base: htmlPath,
            js: uglify,
            css: [cleancss, inlineImages],
            disabledTypes: ['svg', 'img']
        }))
        .pipe(htmlmin({
            collapseWhitespace: true,
            removeComments: true,
            minifyCSS: true,
            minifyJS: true
        }))
        .pipe(gzip())
        .pipe(gulp.dest(dataPath));
})

gulp.task('default', ['buildfs_embeded']);