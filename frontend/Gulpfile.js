var gulp = require('gulp');
var bowerFiles = require('main-bower-files');
var filter = require('gulp-filter');
var concat = require('gulp-concat');
var print = require('gulp-print');
var addSrc = require('gulp-add-src');
var order = require('gulp-order');
var scss = require('gulp-scss');
var bower = require('gulp-bower');
var browserSync = require('browser-sync').create();

// all
gulp.task('default', ['bower', 'js', 'css', 'html']);

// Define default destination folder
var inSourceBuild = true;
var depDir  = "bower_components/";
var srcDir  = "web/";

var destDir = '';
if (inSourceBuild)
    destDir = srcDir;
else
    destDir = 'www/';


// download dependencies
gulp.task('bower', function() {
    return bower(depDir);
});

// js
gulp.task('js', [], function() {

    // prism
    gulp.src(bowerFiles())
        .pipe(filter('**/*prism.js'))
        .pipe(addSrc(depDir + 'prism/components/prism-json.js'))
        .pipe(order([
            'prism.js',
            'prism-json.js'
        ]))
        .pipe(concat('prism.js'))
        .pipe(gulp.dest(destDir + 'js/lib'));

    gulp.src(bowerFiles())
        .pipe(filter(['**/*.js', '!**/*prism.js']))
        .pipe(gulp.dest(destDir + 'js/lib'));

    if (!inSourceBuild)
    gulp.src(srcDir + 'js/*.js')
        .pipe(gulp.dest(destDir + 'js'));
});


// css
gulp.task('css', [], function() {
    // compile materialize
    gulp.src(srcDir +'css/materialize/_variables.scss')
        .pipe(gulp.dest(depDir + 'materialize/sass/components/'));

    gulp.src(depDir + 'materialize/sass/materialize.scss')
        .pipe(print())
        .pipe(scss())
        .pipe(gulp.dest(destDir + "css/lib"))
        .pipe(print());


    gulp.src(bowerFiles())
        .pipe(filter(['**/*.css', '!**/*materialize.css']))
        .pipe(gulp.dest(destDir + 'css/lib'));

    if (!inSourceBuild)
    gulp.src(srcDir +'css/**/*')
        .pipe(filter(['**/**/*']))
        .pipe(gulp.dest(destDir + 'css'));

});

// html
gulp.task('html', [], function() {
    if (!inSourceBuild)
    gulp.src(srcDir +'index.html')
        .pipe(gulp.dest(destDir));
});


// auto refresh
gulp.task('webserver', ['default'], function() {
    browserSync.init({
        server: {
            baseDir: destDir
        },
    })
});

gulp.task('watch', ['webserver'], function (){
    gulp.watch(srcDir + 'index.html', ['html', browserSync.reload]);
    gulp.watch(srcDir + 'js/*.js', ['js', browserSync.reload]);
    gulp.watch(srcDir + 'css/*.css', ['css', browserSync.reload]);
});