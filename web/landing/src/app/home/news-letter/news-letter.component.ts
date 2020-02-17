import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-news-letter',
  templateUrl: './news-letter.component.html',
  styleUrls: ['./news-letter.component.scss']
})
export class NewsLetterComponent implements OnInit {

  constructor() { }

  ngOnInit() {
  }

  onSubmit(name) {
  	if(!name) return false
  	const form = document.createElement('form');
    const element1 = document.createElement('input');

    form.method = 'POST';
    form.target = '_blank';
    form.action = 'https://pixelstrap.us19.list-manage.com/subscribe/post?u=5a128856334b598b395f1fc9b&amp;id=082f74cbda';

    element1.value = name;
    element1.name = 'EMAIL';
    element1.id = 'mce-EMAIL';
    form.appendChild(element1);

    document.body.appendChild(form);

    form.submit();
  }

}
