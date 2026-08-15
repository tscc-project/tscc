interface Props87 { value: number; label: string }
export const View87 = (p: Props87) => (
  <section data-i="87">
    <h2>{p.label}</h2>
    {p.value > 87 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
