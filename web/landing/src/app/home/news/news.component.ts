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
    image: 'assets/images/news-features.png',
    date: 1587751074000,
    author: Author.Manu,
    titleId: "NEWS.NEXT_FEATURES",
    textId: "NEWS.NEXT_FEATURES_TEXT"
  }, {
    image: 'assets/images/news-construction.png',
    date: 1587578274000,
    author: Author.Manu,
    titleId: "NEWS.UNDER_CONSTRUCTION",
    textId: "NEWS.UNDER_CONSTRUCTION_TEXT"
  }, {
    image: 'assets/images/news-alpha.png',
    date: 1587319074000,
    author: Author.Markus,
    titleId: "NEWS.ALPHA",
    textId: "NEWS.ALPHA_TEXT"
  }, {
    image: 'assets/images/news-website.png',
    date: 1587059874000,
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
    dots: true,
    dotsEach: true,
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
