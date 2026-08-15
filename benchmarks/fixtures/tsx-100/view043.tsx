interface Props43 { value: number; label: string }
export const View43 = (p: Props43) => (
  <section data-i="43">
    <h2>{p.label}</h2>
    {p.value > 43 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
