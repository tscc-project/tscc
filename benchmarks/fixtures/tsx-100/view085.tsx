interface Props85 { value: number; label: string }
export const View85 = (p: Props85) => (
  <section data-i="85">
    <h2>{p.label}</h2>
    {p.value > 85 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
