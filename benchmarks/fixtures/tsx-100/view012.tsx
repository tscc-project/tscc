interface Props12 { value: number; label: string }
export const View12 = (p: Props12) => (
  <section data-i="12">
    <h2>{p.label}</h2>
    {p.value > 12 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
