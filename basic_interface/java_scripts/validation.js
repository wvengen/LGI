var validationErrorMessage = new Object();
validationErrorMessage['required'] = 'This field is required';
validationErrorMessage['numeric'] = 'This field requires a number';
validationErrorMessage['postcode'] = 'This field must hold a Dutch postal code';
validationErrorMessage['pattern'] = 'Pattern incorrect';
validationErrorMessage['email'] = 'Incorrect email address';

var validationFunctions = new Object();
validationFunctions["required"] = isRequired;
validationFunctions["pattern"] = isPattern;
validationFunctions["postcode"] = isPostCode;
validationFunctions["numeric"] = isnumeric;
validationFunctions["email"] = isEmail;

function isRequired(formField) {
	switch (formField.type) {
		case 'file':
		case 'text':
		case 'textarea':
		case 'select-one':
			if (formField.value)
				return true;
			return false;
		case 'radio':
			var radios = formField.form[formField.name];
			for (var i=0;i<radios.length;i++) {
				if (radios[i].checked) return true;
			}
			return false;
		case 'checkbox':
			return formField.checked;
	}	
}

function isPattern(formField,pattern) {
	var pattern = pattern || formField.getAttribute('pattern');
	var regExp = new RegExp("^"+pattern+"$","");
	var correct = regExp.test(formField.value);
	if (!correct && formField.getAttribute('patternDesc'))
		correct = formField.getAttribute('patternDesc');
	return correct;
}

function isPostCode(formField) {
	return isPattern(formField,"\\d{4}\\s*\\D{2}");
}

function isnumeric(formField) {
	return isPattern(formField,"\\d+");
}

function isEmail(formField) {
	return isPattern(formField,"\\w*@\\w*\.\\w{2,4}")
}

function isVisible( obj ) {
	if ( obj.style.display != 'none' ) {
		return true;
	} else {
		return false;
	}
}

function emptyFunction() {
	return true;
}

function validate() {
	var els = this.elements;
	var validForm = true;
	var OK = true;
	var firstError = null;
	for (var i=0;i<els.length;i++) {
		//if (els[i].removeError)
		//	els[i].removeError();
        	els[i].className = els[i].className.replace(/errorMessage/,'');
	        els[i].parentNode.className = els[i].parentNode.className.replace(/errorMessage/,'');
		var req = els[i].getAttribute('validation');
		if (!req) continue;
		var reqs = req.split(' ');
		if (els[i].getAttribute('pattern'))
			reqs[reqs.length] = 'pattern';
		for (var j=0;j<reqs.length;j++) {
			if (!validationFunctions[reqs[j]])
				validationFunctions[reqs[j]] = emptyFunction;
			var element = els[i];
			while ( ( element.parentNode ) ) {
				var visible = isVisible ( element );
				//alert ( element.nodeName + ": " + visible );
				if ( visible != true ) break;
				element = element.parentNode
			}
			if ( visible == true ) {
				//alert ( els[i].nodeName + ": " + visible );
				var OK = validationFunctions[reqs[j]](els[i]);
			}
			if (OK != true) {
				var errorMessage = OK || validationErrorMessage[reqs[j]];
				writeError(els[i],errorMessage)
				validForm = false;
				if (!firstError)
					firstError = els[i];
				break;
			}
		}
	}

	if (!validForm) {
		// alert("Errors have been found");
	}
	return validForm;
	
}

function writeError(obj,message) {
	obj.className += ' errorMessage';
	obj.parentNode.className += ' errorMessage';
	obj.onchange = removeError;
	if (obj.errorMessage || obj.parentNode.errorMessage) return;
	var errorMessage = document.createElement('label');
	errorMessage.className = 'errorMessage';
	errorMessage.setAttribute('for',obj.id);
	errorMessage.setAttribute('htmlFor',obj.id);
	errorMessage.appendChild(document.createTextNode(message));
	obj.parentNode.appendChild(errorMessage);
	obj.errorMessage = errorMessage;
	obj.parentNode.errorMessage = errorMessage;
}

function removeError() {
	this.className = this.className.replace(/errorMessage/,'');
	this.parentNode.className = this.parentNode.className.replace(/errorMessage/,'');
	if (this.errorMessage) {
		this.parentNode.removeChild(this.errorMessage);
		this.errorMessage = null;
		this.parentNode.errorMessage = null;
	}
	this.onchange = null;
}

function addEventSimple(obj,evt,fn) {
        if (obj.addEventListener)
                obj.addEventListener(evt,fn,false);
        else if (obj.attachEvent)
                obj.attachEvent('on'+evt,fn);
}

function removeEventSimple(obj,evt,fn) {
        if (obj.removeEventListener)
                obj.removeEventListener(evt,fn,false);
        else if (obj.detachEvent)
                obj.detachEvent('on'+evt,fn);
}

var W3CDOM = document.createElement && document.getElementsByTagName;

function validateForms() {
	if (!W3CDOM) return;
	var forms = document.forms;
	for (var i=0;i<forms.length;i++) {
		forms[i].onsubmit = validate;
	}
}

addEventSimple(window,'load',validateForms);
