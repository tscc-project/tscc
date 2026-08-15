interface Props80 { value: number; label: string }
export const View80 = (p: Props80) => (
  <section data-i="80">
    <h2>{p.label}</h2>
    {p.value > 80 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
