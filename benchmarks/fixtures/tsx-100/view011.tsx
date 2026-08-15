interface Props11 { value: number; label: string }
export const View11 = (p: Props11) => (
  <section data-i="11">
    <h2>{p.label}</h2>
    {p.value > 11 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
