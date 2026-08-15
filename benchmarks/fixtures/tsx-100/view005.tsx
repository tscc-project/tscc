interface Props5 { value: number; label: string }
export const View5 = (p: Props5) => (
  <section data-i="5">
    <h2>{p.label}</h2>
    {p.value > 5 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
