interface Props51 { value: number; label: string }
export const View51 = (p: Props51) => (
  <section data-i="51">
    <h2>{p.label}</h2>
    {p.value > 51 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
