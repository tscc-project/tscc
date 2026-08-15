interface Props44 { value: number; label: string }
export const View44 = (p: Props44) => (
  <section data-i="44">
    <h2>{p.label}</h2>
    {p.value > 44 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
