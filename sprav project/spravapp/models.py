from django.db import models
from django.contrib.auth import get_user_model

def city_deleted():
    return get_user_model().objects.get_or_create(username='Неизвестный город')[0]
def country_deleted():
    return get_user_model().objects.get_or_create(username='Неизвестное государство')[0]

# МОДЕЛИ
class City(models.Model):
    city_name = models.CharField(max_length=100)

    def __str__(self):
        return '%s' % (self.city_name)
    
class Country(models.Model):
    state_name = models.CharField(max_length=100)

    def __str__(self):
        return '%s' % (self.state_name)
    
    
class Phonebook(models.Model):  
    first_name = models.CharField(max_length=50)
    last_name = models.CharField(max_length=50)
    organization = models.CharField(max_length=50)
    phone_number = models.CharField(max_length=20)
    email = models.EmailField()
    address = models.CharField(max_length=100)
    city = models.ForeignKey('City',on_delete=models.SET_NULL, null=True)                 
    state = models.ForeignKey('Country',on_delete=models.SET_NULL, null=True)                
    creation_date = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return '%s %s' % (self.first_name, self.last_name)


#class Contacts(models.Model):
    
   # person = models.ForeignKey('Phonebook',on_delete=models.CASCADE, null=True)
   # phone_number = models.CharField(max_length=20)
    #email = models.EmailField()
    
    
   # def __str__(self):
    #    return '%s' % (self.phone_number)

class Meta:
    ordering = ['last_name']

