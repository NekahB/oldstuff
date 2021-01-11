from django import forms
from django.core.exceptions import ValidationError
from spravapp.models import Phonebook
#from spravapp.models import Contacts
from spravapp.models import City
from spravapp.models import Country

#     ВАЛИДАТОРЫ
def validate_int(value):                              
    try:
        value = int(value)
        value = str(value)
    except ValueError:
        raise ValidationError('Требуется численный формат')

#     КАСТОМНЫЕ ФОРМЫ  (НЕ ИСПОЛЬЗОВАЛ)

class CityModelChoiceField(forms.ModelChoiceField):
    def label_from_instance(self, obj):
         return obj.city_name

class StateModelChoiceField(forms.ModelChoiceField):
    def label_from_instance(self, obj):
         return obj.state_name
 

#     ФОРМЫ
class PhonebookForm(forms.ModelForm):

    first_name = forms.CharField(min_length = 3, max_length = 50)
    last_name = forms.CharField(min_length =3, max_length = 50)
    organization = forms.CharField(min_length =3, max_length = 50)
    state = forms.ModelChoiceField(queryset = Country.objects.all().order_by('state_name'), to_field_name="state_name")
    city = forms.ModelChoiceField(queryset = City.objects.all().order_by('city_name'), to_field_name="city_name")
    address = forms.CharField(max_length = 100)

    class Meta:
        model = Phonebook
        fields = ('__all__')
        
#class ContactsForm(forms.ModelForm):
    
  #  phone_number = forms.CharField(min_length = 3, max_length = 50, validators=[validate_int])
  #  email = forms.EmailField()

  #  class Meta:
   #     model = Phonebook
   #     exclude = ['person']

class CityForm(forms.ModelForm):
    
    city_name = forms.CharField(max_length=100)  

    class Meta:
        model = City
        fields = ('__all__')
        


class CountryForm(forms.ModelForm):

    state_name = forms.CharField(max_length=100)  

    class Meta:
        model = Country
        fields = ('__all__')

#class SearchForm(forms.ModelForm):
  #  phone_number = forms.CharField(min_length = 3, max_length = 50, validators=[validate_int])

   # class Meta:
   #     model = Country
    #    exclude = ['person', 'email']  


