interface Props41 { value: number; label: string }
export const View41 = (p: Props41) => (
  <section data-i="41">
    <h2>{p.label}</h2>
    {p.value > 41 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
