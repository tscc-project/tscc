interface Props30 { value: number; label: string }
export const View30 = (p: Props30) => (
  <section data-i="30">
    <h2>{p.label}</h2>
    {p.value > 30 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
