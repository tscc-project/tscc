interface Props10 { value: number; label: string }
export const View10 = (p: Props10) => (
  <section data-i="10">
    <h2>{p.label}</h2>
    {p.value > 10 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
