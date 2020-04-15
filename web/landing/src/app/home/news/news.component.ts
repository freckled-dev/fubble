import { Component, OnInit } from '@angular/core';
import { Author } from 'src/app/entities/author';

@Component({
  selector: 'app-news',
  templateUrl: './news.component.html',
  styleUrls: ['./news.component.scss']
})
export class NewsComponent implements OnInit {

  public currentNews: any;

  public news = [{
    image: 'assets/images/news-construction.png',
    date: 1586439292739,
    author: Author.Manu,
    titleId: "NEWS.UNDER_CONSTRUCTION",
    textId: "NEWS.UNDER_CONSTRUCTION_TEXT"
  }, {
    image: 'assets/images/news-alpha.png',
    date: 1586124000000,
    author: Author.Markus,
    titleId: "NEWS.ALPHA",
    textId: "NEWS.ALPHA_TEXT"
  }, {
    image: 'assets/images/news-website.png',
    date: 1585692000000,
    author: Author.Manu,
    titleId: "NEWS.WEBSITE",
    textId: "NEWS.WEBSITE_TEXT"
  }]

  // Team Carousel Options
  public newsCarousel: any = {
    loop: false,
    margin: 15,
    nav: false,
    autoplay: false,
    autoplayTimeout: 2000,
    autoplayHoverPause: true,
    dots: false,
    responsive: {
      0: {
        items: 1
      },
      576: {
        items: 2,
        margin: 12
      },
      600: {
        items: 2
      },
      767: {
        items: 2
      },
      768: {
        items: 2,
        margin: 15
      },
      992: {
        items: 3
      },
      1000: {
        items: 3
      }
    }
  }

  constructor() { }

  ngOnInit() {
  }

  showDetails(item: any) {
    this.currentNews = item;
  }
}
