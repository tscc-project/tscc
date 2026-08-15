interface Props90 { value: number; label: string }
export const View90 = (p: Props90) => (
  <section data-i="90">
    <h2>{p.label}</h2>
    {p.value > 90 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
