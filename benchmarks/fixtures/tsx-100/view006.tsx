interface Props6 { value: number; label: string }
export const View6 = (p: Props6) => (
  <section data-i="6">
    <h2>{p.label}</h2>
    {p.value > 6 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
