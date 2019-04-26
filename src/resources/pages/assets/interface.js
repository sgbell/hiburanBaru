function secondsToHMS(s){
    return application.secondsToHMS(s);
}

var application = (function(app){
    app.currentScreen=undefined;
    app.refreshId=undefined;

    app.secondsToHMS=function(s){
        var h = Math.floor(s/3600);
        s -= h*3600;
        var m = Math.floor(s/60);
        s -= m*60;
        return h+":"+(m<10? '0'+m : m)+":"+(s<10? '0'+s : s);
    };

    app.startStatusRefresh=function(){ 
        app.refreshId=setInterval(function(){
            $.get('hibu://status', {}, function(data){
                if ($('.main-window div.status').length==0){
                    statusText=`<div class="status hidden">
                          <h3 hibu-tag="artist">data.artist</h3>
                          <h3 hibu-tag="album">data.album</h3>
                          <h1 hibu-tag="songTitle">data.songTitle</h1>
                          <article><img hibu-tag="albumArt" hibu-tag-type="img" alt="Album Art" /></article>
                          <div><span hibu-tag="elapsedTime" hibu-tag-function="secondsToHMS"></span> <span hibu-tag="state"></span></div>
                          <div><span hibu-tag="volume" ></span></div>
                    </div>`;
                    $(".main-window").html(statusText);
                }
                $.each(data, function(fieldName,value){
                    target = $('.main-window div.status [hibu-tag='+fieldName+']');
                    if (fieldName=="fullPath"){
                        target = $('.main-window div.status [hibu-tag=albumArt]');
                        value="hibu://albumArt/"+value;
                    }
                    if ($(target).attr('hibu-tag-function')!=undefined &&
                        $(target).attr('hibu-tag-function')!=''){
                        fn=window[$(target).attr('hibu-tag-function')];
                        if (typeof fn === "function"){
                            value=fn(value);
                        }
                    }
                    if ($(target).attr('hibu-tag-type')!=undefined &&
                        $(target).attr('hibu-tag-type')!=''){
                        tagType = $(target).attr('hibu-tag-type');
                        if (tagType=='img' &&
                            $('target').attr('src')!=value){
                            $(target).attr('src',value);
                        }
                    } else {
                        $(target).html(value);
                    }
                });
                $('div.status.hidden').removeClass('hidden');
            },
            'json');
        },1000);
    };

    app.initialize = function(){
        $(document).ajaxError(function(event, request, settings){
            $('.main-window').append('<p>Error requesting :'+settings.url+'</p>');
            $('.main-window').append('<p>Request'+request+'</p>');
        });
        left = ($('body').width()/2)-($('body img').width()/2)       
        $('div.loading img').css('left',left);
        $(document.body).on('click', '[hb-action]', function(event){
            var element = $(this);
            if (app.currentScreen!=$(element).attr('hb-action')){
                $.get("hibu:"+$(element).attr('hb-action'), {}, function(data){
                    if ($(element).attr('hb-target')!=undefined){
                        app.currentScreen=$(element).attr('hb-action');
                        clearInterval(app.refreshId);
                        $($(element).attr('hb-target')).html(data);
                    }
	            });
            } else {
                app.currentScreen=undefined;
                app.startStatusRefresh();
            }
        });
        $(document.body).on('click','div.folder li h2', function(event){
           var folder="";
           if ($(this).closest('li').data("parentDir")==true){
               folder = $('ul.library ul[data-full-path="'+$(this).data('fullPath')+'"]').parent().closest("ul[data-full-path]").first().html();
	       if (folder==undefined){
               folder = $('ul.library ul[data-full-path="'+$(this).data('fullPath')+'"]').parent().closest("ul").first().html();
	       }
           } else {
               folder = $('ul.library ul[data-full-path="'+$(this).data('fullPath')+'"]').first().html();
           }
           $('.main-window div.folder').remove();
           $('.main-window').append('<div class="folder" ><ul>'+folder+'</ul></div>');
           $('.main-window div.folder ul li ul').remove();
       });
       $(document.body).on('click','div.main-window ul.playlist li', function(event){
           var element = $(this);
           $.get("hibu://playlist/"+encodeURI($(element).data("song-id")), {}, function(data){
               $(".main-window").html(data);
           });
       });
       app.startStatusRefresh();
   };

   return app;
})({});

$(document).ready(function(){
    application.initialize();
});
