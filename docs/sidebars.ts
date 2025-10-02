import type {SidebarsConfig} from '@docusaurus/plugin-content-docs';

const sidebars: SidebarsConfig = {
  gettingStartedSidebar: [
    {
      type: 'category',
      label: '🚀 Getting Started',
      collapsible: false,
      items: [
        'getting-started/welcome',
      ],
    },
  ],

  guidesSidebar: [
    {
      type: 'category',
      label: '📚 Guides',
      collapsible: false,
      items: [
        'guides/setup',
      ],
    },
  ],
};

export default sidebars;