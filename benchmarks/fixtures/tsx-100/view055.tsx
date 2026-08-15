interface Props55 { value: number; label: string }
export const View55 = (p: Props55) => (
  <section data-i="55">
    <h2>{p.label}</h2>
    {p.value > 55 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
