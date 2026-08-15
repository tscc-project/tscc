interface Props98 { value: number; label: string }
export const View98 = (p: Props98) => (
  <section data-i="98">
    <h2>{p.label}</h2>
    {p.value > 98 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
