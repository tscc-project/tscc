interface Props23 { value: number; label: string }
export const View23 = (p: Props23) => (
  <section data-i="23">
    <h2>{p.label}</h2>
    {p.value > 23 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
