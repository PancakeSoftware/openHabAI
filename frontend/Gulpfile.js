var gulp = require('gulp');
var bowerFiles = require('main-bower-files');
var filter = require('gulp-filter');
var concat = require('gulp-concat');
var print = require('gulp-print');
var rename = require('gulp-rename');
var addSrc = require('gulp-add-src');
var order = require('gulp-order');
var scss = require('gulp-scss');
var browserSync = require('browser-sync').create();

// all
gulp.task('default',['js', 'css', 'html']);

// Define default destination folder
var destDir = 'www/';
var depDir  = "dependencies/";



// js
gulp.task('js', function() {

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

    gulp.src('js/*.js')
        .pipe(gulp.dest(destDir + 'js'));
});


// css
gulp.task('css', function() {
    // compile materialize
    gulp.src('css/materialize/_variables.scss')
        .pipe(gulp.dest(depDir + 'materialize/sass/components/'));

    gulp.src(depDir + 'materialize/sass/materialize.scss')
        .pipe(print())
        .pipe(scss())
        .pipe(gulp.dest(destDir + "css/lib"))
        .pipe(print());


    gulp.src(bowerFiles())
        .pipe(filter(['**/*.css', '!**/*materialize.css']))
        .pipe(gulp.dest(destDir + 'css/lib'));

    gulp.src('css/**/*')
        .pipe(filter(['**/**/*']))
        .pipe(gulp.dest(destDir + 'css'));

});

// html
gulp.task('html', function() {
    gulp.src('index.html')
        .pipe(gulp.dest(destDir));
});


// auto refresh
gulp.task('browserSync', ['default'], function() {
    browserSync.init({
        server: {
            baseDir: destDir
        },
    })
});

gulp.task('watch', ['browserSync'], function (){
    gulp.watch('js/*.js', ['js']);
    gulp.watch('index.html', ['html']);

    gulp.watch('index.html', browserSync.reload);
    gulp.watch('js/*.js', browserSync.reload);
});