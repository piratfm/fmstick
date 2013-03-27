
function buttonClick ()
{
    var div = document.getElementById ("myDiv");
    var button = document.getElementById ("myButton");
    var content_div = document.getElementById ("ListContent");
    var textarea = document.getElementById ("myTextarea");

    if (div.contentEditable == "true")
    {
      div.contentEditable = "false";
      content_div.style.display = "inline";
      textarea.innerHTML = div.innerHTML;
      button.value = "Редактировать";
    }

    else
    {
      div.contentEditable = "true";
      content_div.style.display = "none";
      button.value = "Сохранить";
    }

}
