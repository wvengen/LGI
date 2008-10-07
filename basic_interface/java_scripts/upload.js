function FileUploadChanged()
{
 var this_form = document.getElementById( 'this_form' );
 var usedFields = 0;
 var fields = new Array();

 for( var i = 0; i < this_form.elements.length; i++ )
  if( ( this_form.elements[ i ].type == 'file'  ) && ( this_form.elements[ i ].name.substr( 0, 14 ) == 'uploaded_file_' ) )
   fields[ fields.length ] = this_form.elements[ i ];

 for( var i = 0; i < fields.length; i++ )
  if( fields[ i ].value.length > 0 ) usedFields++;

 this_form.number_of_uploaded_files.value = fields.length;

 if( usedFields == fields.length )
 {
  var lastEntry = document.getElementById( 'file_upload_entry_' + usedFields );

  if( lastEntry )
  {
   var newEntry = document.createElement( 'div' );
   newEntry.id = 'file_upload_entry_' + ( usedFields + 1 );

   var file = document.createElement( 'input' );
   file.type = 'file';
   file.name = 'uploaded_file_' + ( usedFields + 1 );
   file.onchange = function() { FileUploadChanged(); };
   file.size = 75;

   newEntry.appendChild( file );
   lastEntry.parentNode.insertBefore( newEntry, lastEntry.nextSibling );
  }
 }
}

